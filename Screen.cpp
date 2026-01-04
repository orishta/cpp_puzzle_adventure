
#define NOMINMAX // to avoid min/max macro conflicts on Windows (ai suggested this comment)
#include "Screen.h"
#include "utils.h"
#include "console.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm> //  min, max
#include <stdexcept>
#include <cstdio>
// console.h provides cross-platform file utilities; no platform-specific includes here

bool Screen::loadScreenFiles() {
    screenFiles = findScreenFiles();
    // Ensure lexicographic sorting for correct level order (e.g., level_01, level_02, ...)
    std::sort(screenFiles.begin(), screenFiles.end());
    return !screenFiles.empty();
}

const std::string& Screen::getScreenFilename(int index) const {
    static std::string empty;
    if (index < 0 || index >= (int)screenFiles.size()) return empty;
    return screenFiles[index];
}

using namespace GameConstants;

Screen::Screen() : currentMapIndex(0), legendPos(0, 0) {
    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            board[y][x] = EMPTY;
        }
        board[y][MAX_X] = '\0';
    }
}

bool Screen::setMap(int index) {
    if (index < 0 || index >= (int)screenFiles.size()) return false;
    currentMapIndex = index;
    return loadMap(screenFiles[index]);
}

bool Screen::loadMap(const std::string& filename) {
    keys.clear();
    obstacles.clear();
    torches.clear();
    switches.clear();
    springs.clear();
    lastError.clear();

    std::ifstream file(filename);
    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            board[y][x] = EMPTY;
        }
    }
    if (!file) {
        lastError = "Cannot open file: " + filename;
        return false;
        // Or: throw std::runtime_error("Cannot open file: " + filename);
    }
    std::string line;
    int row = 0;
    while (std::getline(file, line) && row < MAX_Y) {
        for (int col = 0; col < (int)line.length() && col < MAX_X; ++col) {
            char c = line[col];
            board[row][col] = c;
            if (c == KEY) keys.emplace_back(col, row);
            else if (c == OBSTACLE) obstacles.emplace_back(col, row);
            else if (c == TORCH) torches.emplace_back(col, row);
            else if (c == SPRING) springs.emplace_back(col, row);
            else if (c == SWITCH_OFF || c == SWITCH_ON) {
                int group = 0;
                if (col + 1 < (int)line.length() && line[col + 1] >= '0' && line[col + 1] <= '9') {
                    group = line[col + 1] - '0';
                    board[row][col + 1] = EMPTY;
                    col++;
                }
                switches.emplace_back(col - (group > 0 ? 1 : 0), row, group, c == SWITCH_ON);
            }
            else if (c == 'L') {
                legendPos = Point(col, row);
                board[row][col] = EMPTY;
            }
        }
        row++;
    }
    // Continue scanning remaining lines for a legend marker 'L' (some maps put it after the map)
    while (std::getline(file, line)) {
        for (int col = 0; col < (int)line.length(); ++col) {
            if (line[col] == 'L') {
                int yPos = std::min(row, MAX_Y - 1);
                int xPos = std::min(col, MAX_X - 1);
                legendPos = Point(xPos, yPos);
                // ensure board shows empty at legend position
                board[yPos][xPos] = EMPTY;
            }
        }
        row++;
    }
    file.close();
    return true;
}

void Screen::draw() const {
    for (int y = 0; y < MAX_Y; ++y) {
        gotoxy(0, y);
        for (int x = 0; x < MAX_X; ++x) {
            char c = board[y][x];
            if (c == WALL_X) c = EMPTY;
            Point(x, y, 0, 0, c).draw();
        }
    }
}

void Screen::drawCharOnly(int x, int y) const {
    if (x < 0 || x >= MAX_X || y < 0 || y >= MAX_Y) return;
    if (isLegendArea(Point(x, y))) return;
    char c = board[y][x];
    if (c == WALL_X) c = EMPTY; 

    Point(x, y, 0, 0, c).draw();
}

char Screen::getCharAt(int x, int y) const {
    if (x < 0 || x >= MAX_X || y < 0 || y >= MAX_Y) return WALL;
    return board[y][x];
}

char Screen::getCharAt(const Point& p) const {
    return getCharAt(p.getX(), p.getY());
}

void Screen::setCharAt(int x, int y, char c) {
    if (x < 0 || x >= MAX_X || y < 0 || y >= MAX_Y) return;
    board[y][x] = c;
    drawCharOnly(x, y);
}

void Screen::setCharAt(const Point& p, char c) {
    setCharAt(p.getX(), p.getY(), c);
}

bool Screen::isWall(const Point& p) const {
    char c = getCharAt(p);
    return c == WALL || c == WALL_X;
}

Key* Screen::getKeyAt(const Point& p) {
    for (auto& key : keys) {
        if (key.getPosition().getX() == p.getX() && key.getPosition().getY() == p.getY())
            return &key;
    }
    return nullptr;
}

Obstacle* Screen::getObstacleAt(const Point& p) {
    for (auto& obs : obstacles) {
        if (obs.getPosition().getX() == p.getX() && obs.getPosition().getY() == p.getY())
            return &obs;
    }
    return nullptr;
}

Torch* Screen::getTorchAt(const Point& p) {
    for (auto& t : torches) {
        if (t.getPosition().getX() == p.getX() && t.getPosition().getY() == p.getY())
            return &t;
    }
    return nullptr;
}

void Screen::addKey(const Point& p) { keys.emplace_back(p.getX(), p.getY()); }
void Screen::addTorch(const Point& p) { torches.emplace_back(p.getX(), p.getY()); }

void Screen::removeTorch(const Point& p) {
    for (auto it = torches.begin(); it != torches.end(); ++it) {
        if (it->getPosition().getX() == p.getX() && it->getPosition().getY() == p.getY()) {
            torches.erase(it);
            break;
        }
    }
}

void Screen::removeKey(const Point& p) {
    for (auto it = keys.begin(); it != keys.end(); ++it) {
        if (it->getPosition().getX() == p.getX() && it->getPosition().getY() == p.getY()) {
            keys.erase(it);
            break;
        }
    }
}

Switch* Screen::getSwitchAt(const Point& p) {
    for (auto& sw : switches) {
        if (sw.getPosition().getX() == p.getX() && sw.getPosition().getY() == p.getY())
            return &sw;
    }
    return nullptr;
}

Spring* Screen::getSpringAt(const Point& p) {
    for (auto& spring : springs) {
        if (spring.getPosition().getX() == p.getX() && spring.getPosition().getY() == p.getY())
            return &spring;
    }
    return nullptr;
}

GameObject* Screen::getObjectAt(const Point& p) {
    // Check each object type and return the first match

    GameObject* obj = nullptr;

    // Springs 
    obj = getSpringAt(p);
    if (obj) return obj;

    // Obstacles 
    obj = getObstacleAt(p);
    if (obj) return obj;

    // Collectible items
    obj = getKeyAt(p);
    if (obj) return obj;

    obj = getTorchAt(p);
    if (obj) return obj;

    // Interactive floor tiles
    obj = getSwitchAt(p);
    if (obj) return obj;

    // Doors are handled separately
    return nullptr;
}

void Screen::toggleSwitch(const Point& p) {
    Switch* sw = getSwitchAt(p);
    if (sw) {
        sw->toggle();
        char c = sw->getState() ? SWITCH_ON : SWITCH_OFF;
        setCharAt(p, c);
    }
}

bool Screen::areAllSwitchesOn(int groupId) const {
    for (const auto& sw : switches) {
        if (sw.getGroup() == groupId && !sw.getState())
            return false;
    }
    return true;
}

int Screen::getDoorSwitchGroup(int doorNumber) const {
    return doorNumber;
}


void Screen::refreshArea(int centerX, int centerY, int radius, bool lookAt) const {// redraw area around (centerX, centerY) with given radius
    for (int y = centerY - radius; y <= centerY + radius; ++y) {
        for (int x = centerX - radius; x <= centerX + radius; ++x) {
            if (x < 0 || x >= MAX_X || y < 0 || y >= MAX_Y ) continue;
			if (y == legendPos.getY() || y == statusRow) continue; // skip legend area and bottom info area
            int dx = x - centerX;
            int dy = y - centerY;
            if (dx * dx + dy * dy <= radius * radius) {
                if (lookAt) {
                    Point(x, y, 0, 0, board[y][x]).draw();
                }
                else {
                    drawCharOnly(x, y);
                }
            }
        }
    }
}

void Screen::updateTorchLocation(const Point& oldP, const Point& newP, int radius) const {
    int minX = std::min(oldP.getX(), newP.getX()) - radius;
    int maxX = std::max(oldP.getX(), newP.getX()) + radius;
    int minY = std::min(oldP.getY(), newP.getY()) - radius;
    int maxY = std::max(oldP.getY(), newP.getY()) + radius;

    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            if (x < 0 || x >= MAX_X || y < 0 || y >= MAX_Y) continue;
            if (y == legendPos.getY()) continue;
            int dxNew = x - newP.getX();
            int dyNew = y - newP.getY();
            bool inNew = (dxNew * dxNew + dyNew * dyNew <= radius * radius);

            int dxOld = x - oldP.getX();
            int dyOld = y - oldP.getY();
            bool inOld = (dxOld * dxOld + dyOld * dyOld <= radius * radius);

            if (inNew) {
                Point(x, y, 0, 0, board[y][x]).draw();
            }
            else if (inOld) {
                drawCharOnly(x, y);
            }
        }
    }
}

    bool Screen::isLegendArea(const Point& p) const {
        // Legend occupies one full-width line at legendPos.y
        if (legendPos.getY() < 0 || legendPos.getY() >= MAX_Y) return false;
        return p.getY() == legendPos.getY();
    }