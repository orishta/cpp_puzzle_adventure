#pragma once
#include "Screen.h"
#include "Player.h"
#include "Legend.h"
#include "Bomb.h"
#include <vector>
#include <chrono>// for timing functions

class Game {
private:
    static constexpr int GAME_CYCLE_DELAY_MS = 100; // 100 ms per game cycle

	// player constants
    static constexpr int PLAYER1_INDEX = 0;
    static constexpr int PLAYER2_INDEX = 1;

	// spawn positions
    static constexpr int PLAYER1_SPAWN_X = 10;
    static constexpr int PLAYER1_SPAWN_Y = 10;
    static constexpr int PLAYER2_SPAWN_X = 15;
    static constexpr int PLAYER2_SPAWN_Y = 5;

	// spawn search offsets
    static constexpr int SPAWN_OVERLAP_OFFSET_X = 2;
    static constexpr int SPAWN_OVERLAP_OFFSET_Y = 1;

	// spawn search radius limits
    static constexpr int MAX_SPAWN_SEARCH_RADIUS = 5;
    static constexpr int MIN_SPAWN_SEARCH_RADIUS = 1;

	// map boundary for scanning
    static constexpr int MAP_SCAN_BOUNDARY = 1;

	// timer digit limits
    static constexpr int TIMER_MIN_DIGIT = 0;
    static constexpr int TIMER_MAX_DIGIT = 9;

	// starting map index
    static constexpr int STARTING_MAP_INDEX = 0;

	// input keys
    static constexpr char PLAYER1_DROP_KEY_LOWER = 'e';
    static constexpr char PLAYER1_DROP_KEY_UPPER = 'E';
    static constexpr char PLAYER2_DROP_KEY_LOWER = 'o';
    static constexpr char PLAYER2_DROP_KEY_UPPER = 'O';

	// revive key
    static constexpr char REVIVE_KEY_LOWER = 'r';
    static constexpr char REVIVE_KEY_UPPER = 'R';

    std::vector<std::string> screenFiles;
    std::string initError;

    Screen screen;
    std::vector<Player> players;
    Legend legend;
    bool running;
    bool paused;

    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

	std::chrono::steady_clock::time_point startTime; // time when level started
	std::chrono::steady_clock::time_point pauseStartTime;// time when pause started
	int lastLegendSeconds;  // last recorded seconds for legend update

    std::vector<Bomb> bombs;

    Point findSafeSpawn(int preferredX, int preferredY, int dx, int dy, char ch);
    void placeLegend();
    void spawnBombAt(int x, int y);
    void tryDropBomb(int playerIndex);
	void processExplosion(const Point& center, int radius); // process bomb explosion effects
    void tryRevivePlayer();

    void startLevel(int mapIndex);
	void handleLevelTransition(); // checks if players passed through doors and handles level change
    void displayGameOverScreen(); // helper: display game over and wait for input
    void initializeGameSession(); // helper: setup players and initial game state

    void handleInput(char key);
    void updateBombs();
	void updatePlayers(); // updates all players
    void drawLegend(); 

public:
    Game();
    void drawStatusLine();
    bool init();
    bool hasError() const 
    { 
		return !initError.empty(); // check if there was an initialization error
    }
    std::string getError() const 
    { 
		return initError; // get initialization error message
    }

    int getScreenCount() const 
    { 
		return (int)screenFiles.size(); // number of available screens
    }

    void run();
    void reset();
};