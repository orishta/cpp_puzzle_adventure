#pragma once
#include "GameObject.h"
#include "Point.h"

class Player;
class Screen;

class Key : public GameObject
{
    Point position;

public:
    Key(int x, int y);
    Key(const Point& p);

    // GameObject interface implementation
    Point getPosition() const override { return position; }
    void draw() const override;
    bool handleCollision(Player& player, Screen& screen) override;

   

private:
    bool canCollect(const Player& player) const;
    void collect(Player& player, Screen& screen);
};