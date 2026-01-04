#include "Game.h"
#include "Door.h"
#include "Riddle.h"
#include "utils.h"
#include "console.h"
#include <iostream>
#include <cmath>

// using namespace to avoid prefixing constants
using namespace GameConstants;


Game::Game()
    : running(false), paused(false), lastLegendSeconds(-1){}
bool Game::init()
{
    // Delegate discovery to Screen and then copy results into Game
    if (!screen.loadScreenFiles()) {
        initError = "No screen files found (adv-world*.screen)";
        return false;
    }
    screenFiles = screen.getScreenFiles();

    // Load and validate riddles game must not start without them
    std::string riddleError;
    if (!initRiddles(riddleError)) {
        initError = riddleError;
        return false;
    }

    initError.clear();
    return true;
}



Point Game::findSafeSpawn(int startX, int startY, int dx, int dy, char ch)
{
    // first quick check
    if (screen.getCharAt(startX, startY) == EMPTY)
        return Point(startX, startY, dx, dy, ch);

    const int MAX_RADIUS = MAX_SPAWN_SEARCH_RADIUS;

    // search in radius
    for (int r = MIN_SPAWN_SEARCH_RADIUS; r <= MAX_RADIUS; ++r)
    {
        for (int y = startY - r; y <= startY + r; ++y)
        {
            if (y < 0 || y >= Screen::MAX_Y) continue;
            for (int x = startX - r; x <= startX + r; ++x)
            {
                if (x < 0 || x >= Screen::MAX_X) continue;
                if (screen.getCharAt(x, y) == EMPTY)
                {
                    return Point(x, y, dx, dy, ch);
                }
            }
        }
    }

    // full scan if needed
    for (int y = MAP_SCAN_BOUNDARY; y < Screen::MAX_Y - MAP_SCAN_BOUNDARY; ++y)
    {
        for (int x = MAP_SCAN_BOUNDARY; x < Screen::MAX_X - MAP_SCAN_BOUNDARY; ++x)
        {
            if (screen.getCharAt(x, y) == EMPTY)
            {
                return Point(x, y, dx, dy, ch);
            }
        }
    }

    // default
    return Point(0, 0, dx, dy, ch);
}

void Game::placeLegend()
{// set legend position based on screens legend position
    Point legendPosition = screen.getLegendPosition();
    legend.setPosition(legendPosition.getX(), legendPosition.getY());
    legend.forceRefresh();
    lastLegendSeconds = -1;
}

void Game::spawnBombAt(int x, int y)
{// spawn bomb at given coordinates
    bombs.emplace_back(x, y);
    int t = bombs.back().getTimer();
    char c = (t > TIMER_MIN_DIGIT && t <= TIMER_MAX_DIGIT) ? char('0' + t) : 'o';
    screen.setCharAt(x, y, c);
}

void Game::tryDropBomb(int playerIndex)
{// drop bomb for specified player
    Player& p = players[playerIndex];

    if (p.disposeBomb()) {
        Point pos = p.getPosition();
        spawnBombAt(pos.getX(), pos.getY());
        playSound("bomb_drop");
    }
    else {
        p.disposeElement();
    }
}

void Game::processExplosion(const Point& center, int radius)
{// process bomb explosion effects
    playSound("explosion");

    // clearing of visuals
    for (int dy = -radius; dy <= radius; ++dy) {
        for (int dx = -radius; dx <= radius; ++dx) {
            if (std::abs(dx) <= radius && std::abs(dy) <= radius) {
                int tx = center.getX() + dx;
                int ty = center.getY() + dy;

                if (tx >= 0 && tx < Screen::MAX_X && ty >= 0 && ty < Screen::MAX_Y) {
					if (screen.getCharAt(tx, ty) == KEY ||( screen.getCharAt(tx,ty)>= DOOR_START && screen.getCharAt(tx,ty)<=DOOR_END))
                        continue;
                    if(statusRow==ty)
                        continue;
                    screen.setCharAt(tx, ty, EMPTY);
                }
            }
        }
    }

    //hit players
    for (size_t j = 0; j < players.size(); ++j) {
        players[j].applyExplosion(center, radius);
    }
}

void Game::tryRevivePlayer()
{
    // Determine which player is alive and which is dead
    Player* alivePlayer = nullptr;
    Player* deadPlayer = nullptr;

    if (players[PLAYER1_INDEX].isAlive() && !players[PLAYER2_INDEX].isAlive()) {
        alivePlayer = &players[PLAYER1_INDEX];
        deadPlayer = &players[PLAYER2_INDEX];
    }
    else if (players[PLAYER2_INDEX].isAlive() && !players[PLAYER1_INDEX].isAlive()) {
        alivePlayer = &players[PLAYER2_INDEX];
        deadPlayer = &players[PLAYER1_INDEX];
    }
    else {
        // Both alive or both dead -> can't revive
        return;
    }

    // Check if alive player has enough score
    if (alivePlayer->getScore() >= SCORE_REVIVAL_COST) {
        alivePlayer->loseScore(SCORE_REVIVAL_COST);
        deadPlayer->revive();

        // Respawn the revived player at a safe location
        Point revivedPos = findSafeSpawn(
            deadPlayer->getPosition().getX(),
            deadPlayer->getPosition().getY(),
            0, 0,
            (deadPlayer == &players[PLAYER1_INDEX]) ? PLAYER_1 : PLAYER_2
        );
        deadPlayer->setInitPosition(revivedPos);
        deadPlayer->draw();

        playSound("revive");
        legend.forceRefresh();
    }
}

void Game::updateBombs()
{// update all bombs
    for (size_t i = 0; i < bombs.size(); ) {
        bombs[i].update();

        if (bombs[i].hasExploded()) {
            processExplosion(bombs[i].getPosition(), Bomb::getBlastRadius());
            bombs.erase(bombs.begin() + i);
        }
        else {
            Point bp = bombs[i].getPosition();
            int t = bombs[i].getTimer();
            if (t > TIMER_MIN_DIGIT && t <= TIMER_MAX_DIGIT) {
                screen.setCharAt(bp.getX(), bp.getY(), char('0' + t));
            }
            ++i;
        }
    }
}

void Game::updatePlayers()
{
    for (auto& player : players) {
        player.move();
    }
}

// main loading function 
void Game::startLevel(int mapIndex)
{
    // Load map by filename from vector
    if (mapIndex < 0 || mapIndex >= (int)screenFiles.size()) {
        return;
    }
    
    if (!screen.setMap(mapIndex)) {
        return;
    }
    Door::resetAllDoors();

    // Reset players
    for (auto& player : players) {
        player.resetAfterLevel();
    }
    // find safe place to spot players
    Point nextSpawn1 = findSafeSpawn(PLAYER1_SPAWN_X, PLAYER1_SPAWN_Y, 0, 0, PLAYER_1);
    Point nextSpawn2 = findSafeSpawn(PLAYER2_SPAWN_X, PLAYER2_SPAWN_Y, 0, 0, PLAYER_2);

    //Prevent overlap
    if (nextSpawn2.getX() == nextSpawn1.getX() && nextSpawn2.getY() == nextSpawn1.getY()) {
        nextSpawn2 = findSafeSpawn(nextSpawn1.getX() + SPAWN_OVERLAP_OFFSET_X,
            nextSpawn1.getY() + SPAWN_OVERLAP_OFFSET_Y, 0, 0, PLAYER_2);

        if (nextSpawn2.getX() == nextSpawn1.getX() && nextSpawn2.getY() == nextSpawn1.getY()) {
            nextSpawn2.set(nextSpawn1.getX() + 1, nextSpawn1.getY());
        }
    }

    // Update and draw
    players[PLAYER1_INDEX].setInitPosition(nextSpawn1);
    players[PLAYER2_INDEX].setInitPosition(nextSpawn2);

    screen.draw();

    legend.forceRefresh();
    placeLegend();

    for (auto& player : players) {
        player.draw();
    }
    drawStatusLine();
}

void Game::displayGameOverScreen() {
    playSound("game_over");
    int gameOverScreenIndex = screen.getNumScreens() - 1;
    if (screen.setMap(gameOverScreenIndex)) {
        cls();
        screen.draw();
    }
    sleep_ms(GAME_OVER_DISPLAY_MS);
    while (!check_kbhit()) { sleep_ms(50); }
    get_single_char();
    running = false;
}

void Game::handleLevelTransition()
{
    // Check for Game Over: both players are dead
    if (!players[PLAYER1_INDEX].isAlive() && !players[PLAYER2_INDEX].isAlive()) {
        displayGameOverScreen();
        return;
    }

    // Only transition when both players have passed through a door
    bool bothPlayersReady = !players[PLAYER1_INDEX].isActive() && !players[PLAYER2_INDEX].isActive();
    if (!bothPlayersReady) {
        return;
    }

    // Both players passed through a door - transition to next level
    playSound("level_finish");

    int currentMap = screen.getCurrentMap();
    int nextMap = 0;
    int doorUsed = players[PLAYER1_INDEX].getLastDoorPassed();

    if (doorUsed == -1) {
        doorUsed = players[PLAYER2_INDEX].getLastDoorPassed();
    }

    // Generic door navigation:
    // Door '1' (index 0): from lobby goes forward, from other maps goes back to lobby
    // Door 'N' (index N-1, where N > 1) goes to map N
    if (doorUsed == 0) {  // Door '1'
        if (currentMap == 0) {
            nextMap = 1;  // From lobby, go to first level
        } else {
            nextMap = 0;  // From any level, go back to lobby
        }
    }
    else {
        nextMap = doorUsed + 1;  // Door 'N' goes to map N
    }

    if (nextMap < (int)screenFiles.size()) {
        // Check if transitioning to the game over screen (last screen)
        if (nextMap == (int)screenFiles.size() - 1) {
            displayGameOverScreen();
        }
        else {
            startLevel(nextMap);
        }
    }
    else {
        running = false;
    }
}

void Game::handleInput(char key)
{
    if (key == KEY_ESC)
    { 
        if (!paused)
        {
            paused = true;
            pauseStartTime = std::chrono::steady_clock::now();

            gotoxy(0, statusRow);
            setTextColor(static_cast<int>(Color::Yellow));
            std::cout << "          >>> PAUSED. Press ESC to Continue, H to return to Menu <<<                 ";
            std::cout.flush();
            setTextColor(static_cast<int>(Color::White));
        }
        else {
            paused = false;
            drawStatusLine();
            auto now = std::chrono::steady_clock::now();
            startTime += (now - pauseStartTime);
            legend.forceRefresh();
        }
    }
    //keys while paused
    else if (paused) {
        if (key == 'h' || key == 'H') {
            running = false;
        }
    }
    //keys while playing
    else {
        if (key == PLAYER1_DROP_KEY_LOWER || key == PLAYER1_DROP_KEY_UPPER) {
            tryDropBomb(PLAYER1_INDEX);
        }
        else if (key == PLAYER2_DROP_KEY_LOWER || key == PLAYER2_DROP_KEY_UPPER) {
            tryDropBomb(PLAYER2_INDEX);
        }
        else if (key == REVIVE_KEY_LOWER || key == REVIVE_KEY_UPPER) {
            tryRevivePlayer();
        }

        else if (key == '1') {
            setSoundEnabled(true);
            drawStatusLine();
        }
        else if (key == '0') {
            setSoundEnabled(false);
            drawStatusLine();
        }
        else {
            for (auto& player : players) {
                player.handleKeyPressed(key);
            }
        }
    }
}

void Game::drawLegend()
{
    int seconds; // Time calculation logic assisted by AI

    if (paused) {
        seconds = (int)std::chrono::duration_cast<std::chrono::seconds>(
            pauseStartTime - startTime).count();
    }
    else {
        auto now = std::chrono::steady_clock::now();
        seconds = (int)std::chrono::duration_cast<std::chrono::seconds>(
            now - startTime).count();
    }

    legend.drawLegend(players[PLAYER1_INDEX], players[PLAYER2_INDEX], seconds);

}

void Game::initializeGameSession() {
    cls();
    Door::resetAllDoors();
    resetRiddlesIndex();
    hideCursor();

    if (!screen.setMap(STARTING_MAP_INDEX)) {
        showErrorMessage(std::string("Failed to load map: ") + screen.getLastError());
        return;
    }
    screen.draw();

    Point spawn1 = findSafeSpawn(PLAYER1_SPAWN_X, PLAYER1_SPAWN_Y, MIN_SPAWN_SEARCH_RADIUS, 0, '$');
    Point spawn2 = findSafeSpawn(PLAYER2_SPAWN_X, PLAYER2_SPAWN_Y, 0, MIN_SPAWN_SEARCH_RADIUS, '&');

    players.clear();
    players.reserve(2);
    players.emplace_back(spawn1, "wdxas", &screen);
    players.emplace_back(spawn2, "ilmjk", &screen);

    Player::registerPlayers(players.data(), (int)players.size());

    placeLegend();
    startTime = std::chrono::steady_clock::now();

    for (auto& player : players) {
        player.draw();
    }

    // Draw persistent helper message
    drawStatusLine();

    running = true;
    paused = false;
}
void Game::drawStatusLine() {
    gotoxy(0, statusRow);
    setTextColor(static_cast<int>(Color::Cyan));
    std::cout << "                  Press ESC for pause | Sound["
        << (isSoundEnabled() ? "on" : "off") << "]                                     ";
    std::cout.flush();
    setTextColor(static_cast<int>(Color::White));
}
void Game::run()
{
    if (!init()) {
        std::cerr << "Game init failed: " << initError << std::endl;
        return;
    }
    drawStatusLine();
    initializeGameSession();

    while (running) {
        if (check_kbhit()) {
            handleInput(get_single_char());
        }

        if (!paused) {
            updatePlayers();
            updateBombs();
            handleLevelTransition();

            // draw updates
            for (const auto& bomb : bombs) {
                Point bp = bomb.getPosition();
                int t = bomb.getTimer();
                if (t >=TIMER_MIN_DIGIT && t <= TIMER_MAX_DIGIT) {
                    screen.setCharAt(bp.getX(), bp.getY(), char('0' + t));
                }
            }
            for (auto& player : players) {
                player.draw();
            }
            
        }

        drawLegend();
        sleep_ms(GAME_CYCLE_DELAY_MS);
    }

    cls();
}

void Game::reset()
{
    running = false;
    paused = false;
    bombs.clear();
    players.clear(); // reset
}