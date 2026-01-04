#include "Key.h"
#include "Player.h"
#include "Screen.h"
#include "Constants.h"

using namespace GameConstants;

Key::Key(int x, int y) : position(x, y) {}
Key::Key(const Point& p) : position(p) {}

void Key::draw() const 
{  
    position.draw(KEY);
}

bool Key::handleCollision(Player& player, Screen& screen) {
    // Check if player already holding an item
    if (player.getHeldItemChar() != EMPTY) {
        return true;  // Block movement if holding something
    }

    if (canCollect(player))
    {
        collect(player, screen);

        // Move player to key position
        Point playerPos = player.getPosition();
        char bg = screen.getCharAt(playerPos);
        playerPos.draw(bg);
        player.setInitPosition(position);
        player.draw();

        return true;
    }

    return false;
}



bool Key::canCollect(const Player& player) const {
    return true;
}

void Key::collect(Player& player, Screen& screen) {// collect the key

    player.pickupKey();
    screen.setCharAt(position, EMPTY);
    screen.removeKey(position);

}