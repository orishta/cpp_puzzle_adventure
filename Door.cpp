#include "Door.h"
#include "Player.h"
#include "Screen.h"
#include "Direction.h"
#include "Constants.h"

using namespace GameConstants; // using namespace to avoid prefixing constants

bool Door::openDoors[MAX_DOORS] = { false };// static array to track open state of doors

Door::Door(int number, const Point& pos, int switchGroup, bool needsKey)// constructor
    : doorNumber(number), position(pos), switchGroupId(switchGroup), requiresKey(needsKey)
{
}



void Door::draw() const
{
    char c = (char)('0' + doorNumber);
    position.draw(c);
}

void Door::resetAllDoors()
{// reset all doors to closed state
    for (int i = 0; i < MAX_DOORS; ++i)
        openDoors[i] = false;
}

bool Door::isOpen(char doorChar) // check if a door is open
{
    if (doorChar < DOOR_START || doorChar > DOOR_END) return false;
    int index = doorChar - DOOR_START;
    return openDoors[index];
}

void Door::setOpen(char doorChar, bool open) // set the open state of a door
{
    if (doorChar < DOOR_START || doorChar > DOOR_END) return;
    int index = doorChar - DOOR_START;
    openDoors[index] = open;
}
bool Door::handleCollision(Player& player, Screen& screen) 
{
    // Doors are handled in Player::processDoorEntry()
    return false; 
}