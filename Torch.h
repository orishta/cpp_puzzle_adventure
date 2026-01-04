#pragma once
#include "GameObject.h"
#include "Point.h"

class Player;
class Screen;

class Torch : public GameObject {
    Point position;

public:
    static constexpr int RADIUS = 2;

    Torch(int x, int y);

    // GameObject interface implementation
    Point getPosition() const override { return position; }
    void draw() const override;
    bool handleCollision(Player& player, Screen& screen) override;

private:
    bool canCollect(const Player& player) const;
    void collect(Player& player, Screen& screen);
};