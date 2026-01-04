#pragma once
#include "GameObject.h"
#include "Point.h"
#include "Direction.h"

class Player;
class Screen;

class Obstacle : public GameObject {
    Point position;

public:
    Obstacle(int x, int y);

    // GameObject interface implementation
    Point getPosition() const override { return position; }
    void draw() const override;
    bool handleCollision(Player& player, Screen& screen) override;

private:
    // push complete logic from PlayerObstacle.cpp
    bool canBePushed(int force, Direction dir, const Screen& screen) const;
    void push(Direction dir, Screen& screen);

    // Calculate player's push force (including spring boost and cooperation)
    int calculatePushForce(const Player& player, Direction pushDir, const Screen& screen) const;
};