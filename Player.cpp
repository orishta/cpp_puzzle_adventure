#include "Player.h"
#include "Direction.h"
#include "utils.h"
#include "Constants.h"
#include "Spring.h"      // For Spring::updateLaunch()
#include "GameObject.h"  // For polymorphic object access
#include "Door.h"        // For Door::isOpen/setOpen 
#include <cstring>
#include <cctype>
#include <cmath>
#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include "console.h"
#include <sstream>

using namespace GameConstants;

Player* Player::allPlayers = nullptr;
int Player::totalPlayers = 0;

void Player::resetDoorKeys() { keysCollectedCounter = 0; } // reset collected door keys

void Player::registerPlayers(Player* array, int count) { // register all players
    allPlayers = array;
    totalPlayers = count;
}

Player::Player(const Point& point, const char(&the_keys)[NUM_KEYS + 1], Screen* theScreen, bool alive)
    : screen(theScreen), lives(alive ? 3 : 0), activePlayer(alive)
{// initialize position and keys
    for (auto& p : body) p = point;
    std::memcpy(keys, the_keys, NUM_KEYS * sizeof(keys[0]));

    spring.reset(); // reset spring state
}

void Player::draw() {// draw player on screen
    if (!activePlayer || !screen || !isAlive()) return;
    // Don't draw on the game over screen (last screen)
    if (screen->getCurrentMap() == screen->getNumScreens() - 1) return;
    body[0].draw();
}



bool Player::isBlockedByOtherPlayer(const Point& p) const { // check collision with other players
    if (!allPlayers || totalPlayers <= 1) return false;

    for (int i = 0; i < totalPlayers; ++i) {
        Player* other = &allPlayers[i];
        if (other == this || !other->isActive()) continue;

        if (other->body[0].getX() == p.getX() && other->body[0].getY() == p.getY()) {
            return true;
        }
    }
    return false;
}

void Player::performMoveVisuals(const Point& oldPos, const Point& newPos) // perform visual updates for movement
{
    if (heldItem == ItemType::TORCH) {
        // updte torch for light effect
        screen->updateTorchLocation(oldPos, newPos, Torch::RADIUS);
    }
    else {
        // delete normally
        screen->drawCharOnly(oldPos.getX(), oldPos.getY());
    }
    // draw player at new position
    body[0].draw();
}


void Player::move() {
    // Dont allow movement if player is dead 
    if (!activePlayer || !screen || !isAlive()) return;
    // Dont allow movement on the game over screen 
    if (screen->getCurrentMap() == screen->getNumScreens() - 1) return;

    // Spring launch phase handled by Spring class
    if (Spring::updateLaunch(*this, *screen)) return;

    // Spring release check - if we have compressed energy but aren't moving onto another spring
    if (spring.energy > 0) {
        Point next = body[0];
        next.move();
        char nextChar = screen->getCharAt(next);

        // Release if not moving onto another spring
        if (nextChar != SPRING) {
            Spring::releaseSpring(*this);
            return;  // Release takes precedence over normal movement
        }
    }

    Point next = body[0];
    next.move();

    // If no movement, return
    if (next.getX() == body[0].getX() && next.getY() == body[0].getY()) {
        return;
    }

    char nextChar = screen->getCharAt(next);

    // Check collisions with other players
    if (nextChar == PLAYER_1 || nextChar == PLAYER_2) return;
    if (isBlockedByOtherPlayer(next)) return;
    // Block movement into the legend area
    if (screen->isLegendArea(next)) return;

    GameObject* obj = screen->getObjectAt(next);// polymorphic object access
    if (obj) {
        // Object handles its own collision logic - NO dynamic_cast needed!
        if (obj->handleCollision(*this, *screen)) {
            return;  // Object handled the collision (e.g., pushed obstacle, player moved)
        }
        // Object blocked movement (e.g., unpushable obstacle) - stop player
        body[0].setDirection(Direction::STAY);
        body[0].draw();
        return;
    }

    // Special cases not yet refactored to GameObject pattern
    if (processBombPickup(next, nextChar)) return;
    if (processDoorEntry(next, nextChar)) return;
    if (processRiddle(next, nextChar)) return;

    // Hit wall so stop movement
    if (nextChar == WALL || nextChar == WALL_X) {
        body[0].setDirection(Direction::STAY);
        body[0].draw();

        // Update lighting even when standing still
        if (heldItem == ItemType::TORCH)
            screen->refreshArea(body[0].getX(), body[0].getY(), Torch::RADIUS, true);
    }
    // Normal move
    else {
        Point oldPos = body[0];
        body[0] = next;

        performMoveVisuals(oldPos, body[0]);
    }
}

void Player::handleKeyPressed(char key_pressed) {// handle movement key press
    size_t index = 0;
    for (char k : keys) {
        if (std::tolower((unsigned char)k) == std::tolower((unsigned char)key_pressed)) {
            body[0].setDirection(static_cast<Direction>(index)); // set direction based on key index
            return;
        }
        ++index;
    }
}

void Player::setInitPosition(Point p) {
    body[0].set(p.getX(), p.getY());
    spring.reset(); // reset spring
}

void Player::moveToPositionPreserveSpring(const Point& newPos)
{
    SpringState saved = spring;
    setInitPosition(newPos);
    spring = saved;
}

void Player::resetAfterLevel() { // reset player state after level
    activePlayer = true;
    lastDoorPassed = -1;
    resetDoorKeys();
    body[0].setDirection(Direction::STAY);
    spring.reset(); // reset spring
}

void Player::loseScore(int amount) {// decrease score and handle life loss
    score -= amount;
    if (score < MIN_SCORE) {
        score = MIN_SCORE;
        playSound("lose_life");
        loseLife();
    }
}

void Player::loseLife() {// lose a life and handle death
    if (lives > 0) --lives;
    if (lives == 0) {
        activePlayer = false; // player is dead
        body[0].draw(EMPTY); // erase player from screen
    }
}

void Player::consumeKey() {// consume a key if held
    if (keysCollectedCounter > 0 && heldItem == ItemType::KEY) {
        keysCollectedCounter--;
        heldItem = ItemType::NONE;
    }
}

void Player::stopMovement() { body[0].setDirection(Direction::STAY); }// stop player movement

void Player::applyExplosion(const Point& center, int radius) {// apply explosion effect to player
    if (!activePlayer) return;
    int dx = std::abs(body[0].getX() - center.getX());
    int dy = std::abs(body[0].getY() - center.getY());
    int cheb = (dx > dy) ? dx : dy;

    if (cheb <= radius) {
        loseLife(); // handles erasing player if dead
        playSound("lose_life");
        if (isAlive()) {
            body[0].draw(); // redraw if still alive
        }
    }
}


char Player::getHeldItemChar() const
{// get character representing held item
    switch (heldItem) {
    case ItemType::KEY:  return KEY;
    case ItemType::BOMB: return BOMB;
    case ItemType::TORCH: return TORCH;
    case ItemType::NONE: return EMPTY;
    }
    return ' ';
}

void Player::disposeElement()
{// dispose of held item at current position
    if (!screen) return;
    if (screen->isWall(body[0])) return;

    char here = screen->getCharAt(body[0]);
    if (here != EMPTY) return;

    if (heldItem == ItemType::TORCH && torchCollectedCounter > 0)
    {
        torchCollectedCounter--;
        heldItem = ItemType::NONE;
        screen->setCharAt(body[0], TORCH);
        screen->addTorch(body[0]);
        screen->refreshArea(body[0].getX(), body[0].getY(), Torch::RADIUS);
        return;
    }

    if (heldItem == ItemType::KEY && keysCollectedCounter > 0)
    {
        keysCollectedCounter--;
        heldItem = ItemType::NONE;
        screen->setCharAt(body[0], KEY);
        screen->addKey(body[0]);
    }
}

bool Player::disposeBomb()
{// dispose bomb if held
    if (heldItem != ItemType::BOMB) return false;
    char here = screen->getCharAt(body[0]);
    if (here != EMPTY) return false;

    heldItem = ItemType::NONE;
    return true;
}


bool Player::processBombPickup(const Point& next, char nextChar)
{// process bomb pickup if player moves onto a bomb
    if (nextChar != BOMB) return false;
    if (heldItem != ItemType::NONE) {
        return true;
    }

    heldItem = ItemType::BOMB;
    addScore(SCORE_BOMB_PICKUP);
    playSound("pickup");

    screen->setCharAt(next, EMPTY);

    char bg = screen->getCharAt(body[0]);
    body[0].draw(bg);
    body[0] = next;
    body[0].draw();

    return true;
}

bool Player::processDoorEntry(const Point& next, char nextChar)
{// process door entry if player moves onto a door
    if (nextChar < DOOR_START || nextChar > DOOR_END) return false;

    int doorIndex = (nextChar - DOOR_START);
    if (doorIndex < 0 || doorIndex >= NUM_DOOR_KEYS) return false;

    // Delegate to Door class for state management
    bool isOpen = Door::isOpen(nextChar);

    // Check if switches are required and all are ON
    int switchGroup = screen->getDoorSwitchGroup(doorIndex + 1);
    bool switchesOk = (switchGroup < 0) || screen->areAllSwitchesOn(switchGroup);

    if (!switchesOk) {
        // Switches not activated - block player
        body[0].setDirection(Direction::STAY);
        return true;
    }

    if (keysCollectedCounter > 0 && heldItem == ItemType::KEY && !isOpen)
    {
        keysCollectedCounter--;
        heldItem = ItemType::NONE;
        Door::setOpen(nextChar, true);

        addScore(SCORE_DOOR_PASS);
        lastDoorPassed = doorIndex;

        setActive(false);
        body[0].draw(' ');

        return true;
    }

    if (isOpen) {
        lastDoorPassed = doorIndex;
        setActive(false);
        body[0].draw(' ');
        return true;
    }

    return true;
}