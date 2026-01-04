#include "Torch.h"
#include "Player.h"
#include "Screen.h"
#include "Constants.h"

using namespace GameConstants;

Torch::Torch(int x, int y) : position(x, y) {}

void Torch::draw() const {
    position.draw(TORCH);
}

bool Torch::handleCollision(Player& player, Screen& screen) {
    // Check if player already holding an item
    if (player.getHeldItemChar() != EMPTY) {
        return true;  // Block movement if holding something
    }

    if (canCollect(player))
    {
        collect(player, screen);

        // Move player to torch position
        Point playerPos = player.getPosition();
        char bg = screen.getCharAt(playerPos);
        playerPos.draw(bg);
        player.setInitPosition(position);
        player.draw();

        return true;
    }

    return false;
}

bool Torch::canCollect(const Player& player) const {
    return true;
}

void Torch::collect(Player& player, Screen& screen) {
    player.pickupTorch();

    screen.setCharAt(position, EMPTY);
	screen.removeTorch(position);
    screen.refreshArea(position.getX(), position.getY(), RADIUS);
}