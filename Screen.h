#pragma once
#include <vector>
#include <string>
#include "Constants.h"
#include "Point.h"
#include "GameObject.h"
#include "Key.h"
#include "Obstacle.h"
#include "Torch.h"
#include "Switch.h"
#include "Spring.h"


class Screen {
public:
    static constexpr int MAX_X = 80;
    static constexpr int MAX_Y = 22;
    static constexpr int WINDOW_HEIGHT = 25;

private:
    char board[MAX_Y][MAX_X + 1];
    int currentMapIndex;
    std::string lastError;

    std::vector<std::string> screenFiles;
    std::vector<Key> keys;
    std::vector<Obstacle> obstacles;
    std::vector<Torch> torches;
    std::vector<Switch> switches;
    std::vector<Spring> springs;
    Screen(const Screen&) = delete;
    Screen& operator=(const Screen&) = delete;

    Point legendPos;

public:
    Screen();

    // Screen file management
    bool loadScreenFiles();
    int getNumScreens() const { return static_cast<int>(screenFiles.size()); }
    const std::string& getScreenFilename(int index) const;
    const std::vector<std::string>& getScreenFiles() const { return screenFiles; }

    // Map loading and management
    bool loadMap(const std::string& filename);
    bool setMap(int index);
    int getCurrentMap() const { return currentMapIndex; }
    std::string getLastError() const { return lastError; }
    void updateScreenIndex() { setMap(currentMapIndex + 1); }

    // Board access
    char getCharAt(int x, int y) const;
    char getCharAt(const Point& p) const;
    void setCharAt(int x, int y, char c);
    void setCharAt(const Point& p, char c);

    // Drawing
    void draw() const;
    void drawCharOnly(int x, int y) const;

    // Collision detection
    bool isWall(const Point& p) const;
    bool isLegendArea(const Point& p) const;

    // Object access - POLYMORPHIC METHOD (returns any GameObject type)
    GameObject* getObjectAt(const Point& p);

    // Legacy object access (for specific types when needed)
    Key* getKeyAt(const Point& p);
    Obstacle* getObstacleAt(const Point& p);
    Torch* getTorchAt(const Point& p);
    Switch* getSwitchAt(const Point& p);
    Spring* getSpringAt(const Point& p);

    // Object management
    void addKey(const Point& p);
    void addTorch(const Point& p);
    void removeTorch(const Point& p);
    void removeKey(const Point& p);
    void toggleSwitch(const Point& p);

    // Query methods
    bool areAllSwitchesOn(int groupId) const;
    int getDoorSwitchGroup(int doorNumber) const;
    Point getLegendPosition() const { return legendPos; }

    // Visual effects (torch/vision)
    void refreshArea(int centerX, int centerY, int radius, bool lookAt = true) const;
    void updateTorchLocation(const Point& oldP, const Point& newP, int radius) const;
};