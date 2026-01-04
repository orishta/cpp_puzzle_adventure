#include "Spring.h"
#include "Player.h"
#include "Screen.h"
#include "Obstacle.h"
#include "Direction.h"
#include "Constants.h"
#include "utils.h"

using namespace GameConstants;

Spring::Spring(int x, int y) : position(x, y) {}

Spring::Spring(const Point& p) : position(p) {}

void Spring::draw() const {
    position.draw(SPRING);
}

Spring::LaunchData Spring::calculateLaunch(int energy) {
    LaunchData data;
    data.launch_speed = energy;
    data.launch_turns = energy * energy;
    return data;
}

bool Spring::handleCollision(Player& player, Screen& screen) {
    //Delegate to compression handler 
    return handleCompression(player, screen);
}

bool Spring::handleCompression(Player& player, Screen& screen) {
    Point playerPos = player.getPosition();
    Point next = playerPos;
    next.move();

    char currentChar = screen.getCharAt(playerPos);
    char nextChar = screen.getCharAt(next);

    // spring is a friend of player
    bool shouldRelease = false;

    // Check if player already in compression phase
    if (player.spring.energy > 0)
    {
        bool leavingSpring = (currentChar == SPRING && nextChar != SPRING);

        // Release conditions
        if (nextChar == WALL)
            shouldRelease = true;

        if (playerPos.getDx() == 0 && playerPos.getDy() == 0)
            shouldRelease = true;

        if ((playerPos.getDx() != player.spring.dx || playerPos.getDy() != player.spring.dy) && nextChar != WALL)
            shouldRelease = true;

        if (leavingSpring)
            shouldRelease = true;
    }

    // Release phase - convert spring energy to launch
    if (shouldRelease)
    {
        releaseSpring(player);
        return true;
    }

    // Compression phase if stepping on 'S'
    if (nextChar == SPRING)
    {
        int curDx = playerPos.getDx();
        int curDy = playerPos.getDy();

        // Ignore if not moving
        if (curDx == 0 && curDy == 0)
            return false;

        // First compression step - record direction
        if (player.spring.energy == 0)
        {
            player.spring.dx = curDx;
            player.spring.dy = curDy;
        }

        // Keep compressing only if still moving in same direction
        if (curDx == player.spring.dx && curDy == player.spring.dy)
        {
            player.spring.energy++;

            // Visual update - erase old position
            char bg = screen.getCharAt(playerPos);
            playerPos.draw(bg);

            // Move player to spring position (but preserve spring state!)
            // Save spring state before setInitPosition resets it
            player.moveToPositionPreserveSpring(next);

            return true;
        }
    }

    return false;
}

void Spring::releaseSpring(Player& player) {
    // No direction or no energy - cannot launch
    if (player.spring.energy <= 0 || (player.spring.dx == 0 && player.spring.dy == 0))
    {
        player.spring.reset();
        return;
    }

    // Calculate launch parameters
    LaunchData launch = calculateLaunch(player.spring.energy);
    player.spring.launch_speed = launch.launch_speed;
    player.spring.launch_turns = launch.launch_turns;

    // Launch opposite to compression direction
    player.spring.launch_dx = -player.spring.dx;
    player.spring.launch_dy = -player.spring.dy;

    // Clear compression state
    player.spring.energy = 0;
    player.spring.dx = 0;
    player.spring.dy = 0;
}

bool Spring::updateLaunch(Player& player, Screen& screen) {
    if (player.spring.launch_turns <= 0) {
        // Clear launch state when launch is over
        player.spring.launch_speed = 0;
        player.spring.launch_dx = 0;
        player.spring.launch_dy = 0;
        return false;
    }

    // One game cycle passed
    player.spring.launch_turns--;

    Point playerPos = player.getPosition();

    // User side movement (allowed as long as not opposite to launch)
    int user_dx = playerPos.getDx();
    int user_dy = playerPos.getDy();

    bool isOpposite = (user_dx == -player.spring.launch_dx && user_dy == -player.spring.launch_dy);

    if (!isOpposite && (user_dx != 0 || user_dy != 0)) {
        Point sideMove = playerPos;
        sideMove.setX(sideMove.getX() + user_dx);
        sideMove.setY(sideMove.getY() + user_dy);

        if (!screen.isWall(sideMove)) {
            char bg = screen.getCharAt(playerPos);
            playerPos.draw(bg);

            player.moveToPositionPreserveSpring(sideMove);
            player.draw();
            playerPos = player.getPosition();  // Update after move
        }
    }

    // Apply launch movement (speed can be >1)
    for (int i = 0; i < player.spring.launch_speed; ++i)
    {
        playerPos = player.getPosition();
        Point fly = playerPos;

        fly.setX(fly.getX() + player.spring.launch_dx);
        fly.setY(fly.getY() + player.spring.launch_dy);

        char flyChar = screen.getCharAt(fly);

        // Obstacle collision - let obstacle handle push with spring force
        if (flyChar == OBSTACLE) {
            Obstacle* obs = screen.getObstacleAt(fly);
            if (obs && obs->handleCollision(player, screen)) {
                // Obstacle was pushed, player moved into that spot
                // Continue launch if turns remain
                continue;
            }
            // Could not push obstacle - stop launch
            player.spring.launch_turns = 0;
            break;
        }

        if (flyChar >= DOOR_START && flyChar <= DOOR_END) {
            player.spring.launch_turns = 0;
            break;
        }

        // Player collision - transfer momentum
        // Note: This requires multi-player support which needs refactoring
        if (flyChar == PLAYER_1 || flyChar == PLAYER_2) {
            player.spring.launch_turns = 0;
            break;
        }

        // Wall blocks launch
        if (screen.isWall(fly)) {
            player.spring.launch_turns = 0;
            break;
        }

        // Regular launch step
        char bg = screen.getCharAt(playerPos);
        playerPos.draw(bg);
        player.moveToPositionPreserveSpring(fly);
        player.draw();
    }

    return true;
}

bool Spring::handleCurrentPosition(Player& player, Screen& screen) {
    // Only relevant if player has compressed spring energy
    if (player.spring.energy <= 0)
        return false;

    Point playerPos = player.getPosition();
    Point next = playerPos;
    next.move();

    char currentChar = screen.getCharAt(playerPos);
    char nextChar = screen.getCharAt(next);

    bool shouldRelease = false;

    // Not on a spring anymore - release
    if (currentChar != SPRING) {
        shouldRelease = true;
    }
    // Trying to move into wall - release
    else if (nextChar == WALL) {
        shouldRelease = true;
    }
    // Player stopped moving - release
    else if (playerPos.getDx() == 0 && playerPos.getDy() == 0) {
        shouldRelease = true;
    }
    // Player changed direction (and not hitting wall) - release
    else if ((playerPos.getDx() != player.spring.dx || playerPos.getDy() != player.spring.dy) && nextChar != WALL) {
        shouldRelease = true;
    }
    // Leaving spring tile - release
    else if (currentChar == SPRING && nextChar != SPRING) {
        shouldRelease = true;
    }

    if (shouldRelease) {
        releaseSpring(player);
        return true;
    }

    return false;
}
