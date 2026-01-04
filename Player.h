#pragma once
#include "Point.h"
#include "Screen.h"

// Forward declarations
class Spring;
class Obstacle;

class Player {
    // Friend classes for polymorphic design
    friend class Spring;   // Spring needs access to spring
    friend class Obstacle; // Obstacle needs access to obstacle

    static constexpr int SIZE = 1;
    static constexpr int NUM_KEYS = 5;
    static constexpr int NUM_DOOR_KEYS = 10;

    char    keys[NUM_KEYS];
    Point   body[SIZE];
    int keysCollectedCounter = 0;
    int torchCollectedCounter = 0;
    int score = 0;
    int lives = 3;
    Screen* screen = nullptr;

    // tracks which door we exited from
    int lastDoorPassed = -1;

    enum class ItemType { NONE, KEY, BOMB, TORCH };
    ItemType heldItem = ItemType::NONE;

    // Player active state (false when passed through door or dead)
    bool activePlayer = true;

    // Spring mechanics - fields are private, accessed through friend classes
    class SpringState {
        friend class Player;   // Player can access private fields
        friend class Spring;   // Spring class needs access to manage spring mechanics
        friend class Obstacle; // Obstacle needs spring state for push force calculation
    private:
        int energy = 0;
        int dx = 0;
        int dy = 0;
        int launch_turns = 0;
        int launch_speed = 0;
        int launch_dx = 0;
        int launch_dy = 0;

    public:
        void reset() { *this = SpringState(); }
    };

    SpringState spring;

    // Helper functions for clean logic
    bool isBlockedByOtherPlayer(const Point& p) const;
    void performMoveVisuals(const Point& oldPos, const Point& newPos);

    // Special cases not yet refactored (Riddle, Bomb, Door)
    bool processDoorEntry(const Point& next, char nextChar);
    bool processBombPickup(const Point& next, char nextChar);
    bool processRiddle(const Point& next, char nextChar);
    void handleRiddle(const Point& riddlePos);

    static Player* allPlayers;
    static int totalPlayers;

public:
    // Constructors
    Player() = default;
    Player(const Point& point, const char(&the_keys)[NUM_KEYS + 1], Screen* theScreen, bool alive = true);

    // Delete copy (vector can't copy)
    Player(const Player&) = delete;
    Player& operator=(const Player&) = delete;

    // Allow move (vector needs this!)
    Player(Player&&) = default;
    Player& operator=(Player&&) = default;

    // Getters (const correctness)
    int getKeys() const { return keysCollectedCounter; }
    int getLives() const { return lives; }
    int getScore() const { return score; }
    int getLastDoorPassed() const { return lastDoorPassed; }
    char getHeldItemChar() const;
    Point getPosition() const { return body[0]; }
    bool isAlive() const { return (lives > 0); }
    bool isActive() const { return activePlayer; }
    void setActive(bool active) { activePlayer = active; }

    // Drawing
    void draw();
    void draw(char c) { body[0].draw(c); }

    // Movement and input
    void move();
    void handleKeyPressed(char key);
    void stopMovement();

    // Item management
    void disposeElement();
    bool disposeBomb();
    void consumeKey();
    void pickupKey() {
        heldItem = ItemType::KEY;
        keysCollectedCounter++;
    }
    void pickupTorch() {
        heldItem = ItemType::TORCH;
        torchCollectedCounter++;
    }

    // State management
    void resetDoorKeys();
    void setInitPosition(Point p);
    void moveToPositionPreserveSpring(const Point& newPos);

    void resetAfterLevel();
    static void registerPlayers(Player* array, int count);

    // Score and lives
    void addScore(int amount) { score += amount; }
    void loseScore(int amount);
    void loseLife();
    void gainLife() { ++lives; }

    // Revival system - revive a dead player
    void revive() {
        if (!isAlive()) {
            lives = 1;
            activePlayer = true;
        }
    }

    //Combat
    void applyExplosion(const Point& center, int radius);
};