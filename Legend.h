#pragma once
#include "Player.h"   

class Legend {
    int x, y;
    static constexpr int width = 20;
    static constexpr int height = 3;

    // state tracking (prevents flickering)
    int lastP1Score = -1;
    int lastP1Lives = -1;
    char lastP1Item = ' ';
    bool lastP1Alive = true;

    int lastP2Score = -1;
    int lastP2Lives = -1;
    char lastP2Item = ' ';
    bool lastP2Alive = true;

    int lastTime = -1;
    bool needsRefresh = false;

public:
    void setPosition(int newX, int newY) {
        x = newX;
        y = newY;
        forceRefresh(); // redraw when legend moves
    }

    static constexpr int getWidth() { return width; }
    static constexpr int getHeight() { return height; }

    void drawLegend(const Player& p1, const Player& p2, int gameTime);

    // force redraw on next update
    void forceRefresh() { needsRefresh = true; }

};