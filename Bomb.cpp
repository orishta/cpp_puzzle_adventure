#include "Bomb.h"

Bomb::Bomb(int x, int y)
    : position(x, y),
    timer(EXPLODE_TICKS),
    bombed(false)
{
}

void Bomb::update() {
    if (timer > 0) {
        --timer;
        if (timer == 0) {
            bombed = true;
        }
    }
}