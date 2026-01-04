#include "Point.h"
#include "Screen.h"
#include "utils.h"
#include "console.h"
#include "Direction.h"
#include <iostream>

// Choose color according to the character on the screen
static int colorForChar(char c)
{
    switch (c) {
    case '#': return static_cast<int>(Color::DarkGrey);      // wall
    case 'S': return static_cast<int>(Color::Yellow);        // spring
    case '*': return static_cast<int>(Color::LightRed);      // obstacle
    case 'k': return static_cast<int>(Color::LightCyan);     // key
    case '?': return static_cast<int>(Color::LightMagenta);  // riddle
    case '1': case '2': case '3':
    case '4': case '5': case '6':
    case '7': case '8': case '9':   // doors
        return static_cast<int>(Color::LightBlue);
    case '$': return static_cast<int>(Color::LightGreen);    // player 1
    case '&': return static_cast<int>(Color::LightCyan);     // player 2
    default:  return static_cast<int>(Color::White);
    }
}

Point::Point(int x, int y, int diff_x, int diff_y, char ch)
    : x(x), y(y), diff_x(diff_x), diff_y(diff_y), ch(ch)
{
}

void Point::draw(char c) const {
    gotoxy(x, y);

    // if c==0 → draw the point's own char; otherwise draw c
    char toPrint = (c == 0 ? ch : c);

    int color = colorForChar(toPrint);
    setTextColor(color);

    std::cout << toPrint;
    std::cout.flush();

    setTextColor(static_cast<int>(Color::White)); // back to default so other prints are normal
}

void Point::move() {
    x += diff_x;
    y += diff_y;
    if (x < 0) x = 0;
    if (x >= Screen::MAX_X) x = Screen::MAX_X - 1;
    if (y < 0) y = 0;
    if (y >= Screen::MAX_Y) y = Screen::MAX_Y - 1;
}

void Point::setDirection(Direction dir) {
    switch (dir) {
    case Direction::UP:
        diff_x = 0;  diff_y = -1; break;
    case Direction::RIGHT:
        diff_x = 1;  diff_y = 0;  break;
    case Direction::DOWN:
        diff_x = 0;  diff_y = 1;  break;
    case Direction::LEFT:
        diff_x = -1; diff_y = 0;  break;
    case Direction::STAY:
        diff_x = 0;  diff_y = 0;  break;
    }
}

void Point::set(int new_x, int new_y) {
    x = new_x;
    y = new_y;
}
