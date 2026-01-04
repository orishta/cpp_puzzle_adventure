#pragma once
#include "GameObject.h"
#include "Point.h"
#include "Constants.h"

class Player;// forward declaration
class Screen;

class Door : public GameObject {
    static bool openDoors[GameConstants::MAX_DOORS];
    int doorNumber;
    Point position;
    int switchGroupId = GameConstants::NO_SWITCH_GROUP; // associated switch group
    bool requiresKey = true;// does door require a key to open

public:
    Door(int number, const Point& pos, int switchGroup = GameConstants::NO_SWITCH_GROUP, bool needsKey = true);

    // GameObject interface implementation
    Point getPosition() const override { return position; }
    void draw() const override;
    bool handleCollision(Player& player, Screen& screen) override;// attempt to open the door for the player

    // Door specific methods
    char getChar() const { return (char)((GameConstants::DOOR_START - 1) + doorNumber); }

    // Static door state management
    static void resetAllDoors();
    static bool isOpen(char doorChar);
    static void setOpen(char doorChar, bool open);

};