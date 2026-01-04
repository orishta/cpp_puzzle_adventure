#pragma once
#include "utils.h"
#include "Direction.h"

class Point {
    int x, y;
    int diff_x, diff_y; // The direction
    char ch;

public:
    Point(int x = 0, int y = 0, int diff_x = 0, int diff_y = 0, char ch = '#');

    void set(int x, int y);
    void draw(char c = 0) const;
    void move();
    void setDirection(Direction dir);

    int getDx() const { return diff_x; }
    int getDy() const { return diff_y; }
    int getX() const { return x; }
    int getY() const { return y; }
    char getChar() const { return ch; }
    void setX(int _x) { x = _x; }
    void setY(int _y) { y = _y; }
};
 