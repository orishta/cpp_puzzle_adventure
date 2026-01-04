#include "Switch.h"
#include "Player.h"
#include "Screen.h"
#include "console.h"

using namespace GameConstants;

Switch::Switch(int x, int y, int group, bool initialState)
    : position(x, y, 0, 0, initialState ? SWITCH_ON : SWITCH_OFF),
      isOn(initialState),
      groupId(group)
{
}

void Switch::toggle() {
    isOn = !isOn;
}

void Switch::draw() const {
    char c = isOn ? SWITCH_ON : SWITCH_OFF;
    position.draw(c);
}

bool Switch::handleCollision(Player& player, Screen& screen) {
    // Activate the switch
    activate(player, screen);

    // Move player to switch position
    Point playerPos = player.getPosition();
    Point oldPos = playerPos;
    player.setInitPosition(position);

    // Visual update using performMoveVisuals-like logic
    if (player.getHeldItemChar() == TORCH) {
        screen.updateTorchLocation(oldPos, position, Torch::RADIUS);
    } else {
        screen.drawCharOnly(oldPos.getX(), oldPos.getY());
    }
    player.draw();

    return true;
}

void Switch::activate(Player& player, Screen& screen) {
    toggle();
    char c = isOn ? SWITCH_ON : SWITCH_OFF;
    screen.setCharAt(position, c);
}
