#pragma once
#include "Point.h"

class Player;
class Screen;

class Bomb {
    Point position;
    int timer;
    bool bombed = false;

	static constexpr int EXPLODE_TICKS = 40; // 4 seconds if update() is called
    static constexpr int BLAST_RADIUS = 3; 

public:
    Bomb(int x, int y);

	void update(); // updates the timer and explosion status

	int getTimer() const  // returns time in seconds
    {
        return (timer / 10);
    }

	bool hasExploded() const // returns true if bomb has exploded
    { 
        return bombed;
    }
  
	Point getPosition() const // returns position of the bomb
     { 
        return position; 
     }

	static constexpr int getBlastRadius() // returns blast radius in number of cells
    {
       return BLAST_RADIUS; 
    }




};