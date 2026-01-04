#pragma once
#include "Point.h"

class Player;
class Screen;

class GameObject {
public:
    virtual ~GameObject() = default; // after completion of polymorphic base class
    virtual void draw() const = 0;
    virtual Point getPosition() const = 0;
    virtual bool handleCollision(Player& player, Screen& screen) = 0;
};
