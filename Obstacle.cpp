#include "Obstacle.h"
#include "Player.h"
#include "Screen.h"
#include "Direction.h"
#include "Constants.h"
#include <vector>

using namespace GameConstants; // using namespace to avoid prefixing constants

Obstacle::Obstacle(int x, int y) : position(x, y) {}

void Obstacle::draw() const {
    position.draw(OBSTACLE);
}

bool Obstacle::handleCollision(Player& player, Screen& screen) {
    // Player is trying to move into this obstacle
    Point playerPos = player.getPosition();

    // Calculate push direction from player position to obstacle
    int dx = position.getX() - playerPos.getX();
    int dy = position.getY() - playerPos.getY();

    Direction pushDir = Direction::STAY;
    if (dx == 1) pushDir = Direction::RIGHT;
    else if (dx == -1) pushDir = Direction::LEFT;
    else if (dy == 1) pushDir = Direction::DOWN;
    else if (dy == -1) pushDir = Direction::UP;
    else return false;  // Invalid push direction

    // Calculate total push force 
    int totalForce = calculatePushForce(player, pushDir, screen);

    // Check if obstacle can be pushed with this force
    if (canBePushed(totalForce, pushDir, screen))
    {
        // remember old obstacle position
        Point oldPos = position;

        // push the obstacle 
        push(pushDir, screen);

        // move player into the cleared spot (where obstacle was)
        char bg = screen.getCharAt(playerPos);
        playerPos.draw(bg);

        // If player is being spring preserve the launch state
        player.moveToPositionPreserveSpring(oldPos);
        player.draw();

        return true;  // Collision handled, player moved
    }

    return false;  // Cannot push, block movement
}

int Obstacle::calculatePushForce(const Player& player, Direction pushDir, const Screen& screen) const {
    // Base force
    int force = 1;

    // Spring boost if player has launch speed 
    if (player.spring.launch_speed > 0)
        force = player.spring.launch_speed;

    // Cooperative push check for another player behind this player
    Point playerPos = player.getPosition();

    int dx = 0, dy = 0;
    switch (pushDir) {
    case Direction::UP:    dy = -1; break;
    case Direction::DOWN:  dy = 1; break;
    case Direction::LEFT:  dx = -1; break;
    case Direction::RIGHT: dx = 1; break;
    default: return force;
    }

    // Position behind the player
    int behindX = playerPos.getX() - dx;
    int behindY = playerPos.getY() - dy;

    // Check for other players at that position
    if (Player::allPlayers && Player::totalPlayers > 1)
    {
        for (int i = 0; i < Player::totalPlayers; ++i)
        {
            Player* other = &Player::allPlayers[i];
            if (other == &player) continue;
            if (!other->isActive()) continue;

            Point otherPos = other->getPosition();
            if (otherPos.getX() == behindX && otherPos.getY() == behindY)
            {
                int otherForce = 1;

                // Other player's spring boost
                if (other->spring.launch_speed > 0)
                    otherForce = other->spring.launch_speed;

                force += otherForce;
                break;
            }
        }
    }

    return force;
}


bool Obstacle::canBePushed(int force, Direction dir, const Screen& screen) const  // check if obstacle can be pushed
{
    int dx = 0, dy = 0;

    switch (dir) {
    case Direction::UP:    dy = -1; break;
    case Direction::DOWN:  dy = 1; break;
    case Direction::LEFT:  dx = -1; break;
    case Direction::RIGHT: dx = 1; break;
    default: return false;
    }

    // Count ALL obstacles in the push direction to get obstacle size
    Point cur = position;
    int obstacleSize = 0;

	while (screen.getCharAt(cur) == OBSTACLE) //count all obstacles forming this group
    {
        obstacleSize++;
        cur.setX(cur.getX() + dx);
        cur.setY(cur.getY() + dy);
    }

    // Check if we have enough force for the obstacle size
    if (obstacleSize > force) return false;

	// Check if destination (after all obstacles) is empty
	char destChar = screen.getCharAt(cur);
    if (destChar != EMPTY) return false;

    return true;
}



void Obstacle::push(Direction dir, Screen& screen) // push the obstacle in the specified direction
{
    int dx = 0, dy = 0;
    switch (dir) {
    case Direction::UP:    dy = -1; break;
    case Direction::DOWN:  dy = 1; break;
    case Direction::LEFT:  dx = -1; break;
    case Direction::RIGHT: dx = 1; break;
    default: return;
    }

    // collect all obstacles in the chain before modifying the screen
    std::vector<Obstacle*> obstacleChain;
    Point cur = position;
    while (screen.getCharAt(cur) == OBSTACLE) // count obstacles in the push direction
    {
        Obstacle* obs = screen.getObstacleAt(cur);
        if (obs) {
            obstacleChain.push_back(obs);
        }
        cur.setX(cur.getX() + dx);
        cur.setY(cur.getY() + dy);
    }

    int blockLen = static_cast<int>(obstacleChain.size());

	for (int i = 0; i < blockLen; i++) // move obstacles one by one
    {
        Point dest = cur;
        dest.setX(dest.getX() - i * dx);
        dest.setY(dest.getY() - i * dy);

        Point src = dest;
        src.setX(src.getX() - dx);
        src.setY(src.getY() - dy);

        screen.setCharAt(dest, OBSTACLE);
        screen.setCharAt(src, EMPTY);
    }

    for (size_t i = 0; i < obstacleChain.size(); i++) {// update obstacle positions
        int oldX = obstacleChain[i]->position.getX();
        int oldY = obstacleChain[i]->position.getY();
        obstacleChain[i]->position.setX(oldX + dx);
        obstacleChain[i]->position.setY(oldY + dy);
    }
}