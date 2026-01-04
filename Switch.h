#pragma once
#include "GameObject.h"
#include "Point.h"
#include "Constants.h"

class Player;
class Screen;

class Switch : public GameObject {
    Point position;
    bool isOn;
    int groupId;

public:
    Switch(int x, int y, int group, bool initialState = false);

    // GameObject interface implementation
    Point getPosition() const override { return position; }
    void draw() const override;
    bool handleCollision(Player& player, Screen& screen) override;

    // Switch-specific methods
    void toggle();
    bool getState() const { return isOn; }
    int getGroup() const { return groupId; }

private:
    void activate(Player& player, Screen& screen);
};
