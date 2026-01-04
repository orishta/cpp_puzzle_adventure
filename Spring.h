#pragma once
#include "GameObject.h"
#include "Point.h"

class Player;
class Screen;

class Spring : public GameObject {
    Point position;

public:
    Spring(int x, int y);
    Spring(const Point& p);

    // GameObject interface implementation
    Point getPosition() const override { return position; }
    void draw() const override;
    bool handleCollision(Player& player, Screen& screen) override;

    // Spring mechanics - manages player's spring state during launch phase
    static bool updateLaunch(Player& player, Screen& screen);

    // Handle spring state when player is currently on a spring
    static bool handleCurrentPosition(Player& player, Screen& screen);

    // Release compressed spring energy
    static void releaseSpring(Player& player);

private:
    // Internal physics calculations
    struct LaunchData {
        int launch_speed;
        int launch_turns;
    };

    static LaunchData calculateLaunch(int energy);

    // Compression logic
    bool handleCompression(Player& player, Screen& screen);
};
