#include "Legend.h"
#include "utils.h"
#include "Constants.h"
#include "console.h"
#include <iostream>
#include <iomanip> // for std::setw and std::setfill


using namespace GameConstants;

void Legend::drawLegend(const Player& p1, const Player& p2, int gameTime)
{// draw the legend with player stats and game time
    bool changeP1 = needsRefresh || (p1.getScore() != lastP1Score ||
        p1.getLives() != lastP1Lives ||
        p1.getHeldItemChar() != lastP1Item ||
        p1.isAlive() != lastP1Alive);

    bool changeP2 = needsRefresh || (p2.getScore() != lastP2Score ||
        p2.getLives() != lastP2Lives ||
        p2.getHeldItemChar() != lastP2Item ||
        p2.isAlive() != lastP2Alive);

    bool changeTime = needsRefresh || (gameTime != lastTime);

    if (!changeP1 && !changeP2 && !changeTime) return;

    needsRefresh = false;

    lastP1Score = p1.getScore();
    lastP1Lives = p1.getLives();
    lastP1Item = p1.getHeldItemChar();
    lastP1Alive = p1.isAlive();
    lastP2Score = p2.getScore();
    lastP2Lives = p2.getLives();
    lastP2Item = p2.getHeldItemChar();
    lastP2Alive = p2.isAlive();
    lastTime = gameTime;


    if (changeTime) {// update game time display
        gotoxy(x, y);
        std::cout << "Time: " << std::setw(3) << std::setfill('0') << gameTime << "      ";
    }
   
    if (changeP1) {// update player 1 stats display
        gotoxy(x, y + 1);
        if (p1.isAlive())
        {
            std::cout << "P1: " << std::setw(3) << std::setfill('0') << p1.getScore()
                << " L:" << p1.getLives() << " [" << p1.getHeldItemChar() << "]                      ";
        }
        else {
			std::cout << "P1 IS DEAD PRESS R TO REVIVE (100 SC)";
        }
    }

    if (changeP2) {// update player 2 stats display
        gotoxy(x, y + 2);
        if (p2.isAlive()) {
            std::cout << "P2: " << std::setw(3) << std::setfill('0') << p2.getScore()
                << " L:" << p2.getLives() << " [" << p2.getHeldItemChar() << "]                      ";
        }
        else {
            std::cout << "P2 IS DEAD PRESS R TO REVIVE (100 SC)";
        }
    }

    setTextColor(static_cast<int>(Color::White)); 
}

