#pragma once

namespace GameConstants 
{
    // Map
    constexpr char WALL = '#';
    constexpr char WALL_X = 'X';
    constexpr char EMPTY = ' ';

    // Players
    constexpr char PLAYER_1 = '$';
    constexpr char PLAYER_2 = '&';
    constexpr int INITIAL_LIVES = 3;

    // Objects
    constexpr char KEY = 'k';
    constexpr char BOMB = 'b';
    constexpr char TORCH = 'T';
    constexpr char OBSTACLE = '*';
    constexpr char RIDDLE = '?';
    constexpr char SPRING = 'S';
    constexpr char SWITCH_OFF = '-';
    constexpr char SWITCH_ON = '=';

    // Doors
    constexpr char DOOR_START = '1';
    constexpr char DOOR_END = '9';
    constexpr int MAX_DOORS = 10;
    constexpr int DOOR_INDEX_OFFSET = 1;
    constexpr int NO_SWITCH_GROUP = -1;

    // Scoring
    constexpr int SCORE_BOMB_PICKUP = 10;
    constexpr int SCORE_DOOR_PASS = 100;
    constexpr int SCORE_RIDDLE_CORRECT = 50;
    constexpr int SCORE_RIDDLE_PENALTY = 10;
    constexpr int SCORE_REVIVAL_COST = 100;
    constexpr int MIN_SCORE = 0;

    // General settings
    constexpr char KEY_ESC = 27;
    constexpr int NUM_MOVEMENT_KEYS = 5;
    constexpr int CONSOLE_WIDTH = 80;
    constexpr int CONSOLE_HEIGHT = 25;
    constexpr int INPUT_CHECK_DELAY_MS = 50;
   

    // UI Layout
    constexpr int LEGEND_P2_OFFSET = 32;
    constexpr int LEGEND_TIME_OFFSET = 65;
    constexpr int LEGEND_CLEAR_WIDTH = 34;
    constexpr int MAX_LIVES_DISPLAY = 3;
    constexpr int HEART_SYMBOL_WIDTH = 3;
    const int statusRow = 25;


    // Sound and feedback
    constexpr int SOUND_FEEDBACK_DELAY_MS = 800;
    constexpr int ERROR_BOX_Y = 10;

    // Riddles
    constexpr int RIDDLE_FEEDBACK_DELAY_MS = 900;
    constexpr int GAME_OVER_DISPLAY_MS = 2000;
    constexpr int RIDDLE_MIN_WIDTH = 30;
    constexpr int RIDDLE_PADDING = 4;
    constexpr int RIDDLE_BOX_WIDTH_EXTRA = 4;
    constexpr int RIDDLE_BOX_HEIGHT_EXTRA = 5;
    constexpr int RIDDLE_BOUNDARY_OFFSET = 2;
}
