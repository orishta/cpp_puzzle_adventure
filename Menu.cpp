#include "Menu.h"
#include "Game.h"
#include "console.h"
#include "utils.h"
#include "Constants.h"
#include <iostream>

using std::cout;
using std::endl;

void Menu::run()
{
    while (true)
    {
        displayMainMenu();

        while (check_kbhit()) get_single_char(); // Clear input buffer
        char choice = '\0';

        while (!check_kbhit()) // Wait for user input
        {
            sleep_ms(50);
        }

        choice = static_cast<char>(get_single_char());
        handleChoice(choice);

        if (choice == '9') // Exit
        {
            cout << "Goodbye!" << endl;
            break;
        }
    }
}

void Menu::displayMainMenu()
{
    clrscr();
    setTextColor(Color::White);
    cout << "(1) Start game (Colors)\n";
    cout << "(2) Start game (No Colors)\n";
    cout << "(3) Toggle Sound [" << (isSoundEnabled() ? "ON" : "OFF") << "]\n";
    cout << "(8) Instructions\n";
    cout << "(9) EXIT\n\n";
    cout << "Choice: ";
    cout.flush();
}

void Menu::handleChoice(char choice)
{
    if (choice == '1')
    {
        launchGame(true);
    }
    else if (choice == '2')
    {
        launchGame(false);
    }
    else if (choice == '3')
    {
        setSoundEnabled(!isSoundEnabled());
        clrscr();
        setTextColor(Color::Yellow);
        cout << "Sound is now " << (isSoundEnabled() ? "ON" : "OFF") << endl;
        sleep_ms(GameConstants::SOUND_FEEDBACK_DELAY_MS);
    }
    else if (choice == '8')
    {
        printInstructions();
    }
}

void Menu::printInstructions()
{
    clrscr();
    setTextColor(Color::White);
    cout << "Instructions:\n\n";
    cout << "Player 1 ($) - WASD to move, E to drop.\n";
    cout << "Player 2 (&) - IJKL to move, O to drop.\n";
    cout << "Collect Keys (k) to open Doors (1-9).\n";
    cout << "Collect Torches (T) to reveal secret walls (X -> #).\n";
    cout << "Beware of Bombs (b)!\n\n";

    setTextColor(Color::Yellow);
    cout << "REVIVAL SYSTEM:\n";
    setTextColor(Color::White);
    cout << "If one player dies, the other can revive them!\n";
    cout << "Press R to revive your friend for 100 score points.\n\n";

    setTextColor(Color::Cyan);
    cout << "SOUND CONTROL:\n";
    setTextColor(Color::White);
    cout << "Press 1 during gameplay to turn sound ON - Sound[1]\n";
    cout << "Press 0 during gameplay to turn sound OFF - Sound[0]\n\n";

    cout << "Press any key to return to the main menu.\n";

    while (!check_kbhit()) { sleep_ms(50); }
    get_single_char();
}

void Menu::launchGame(bool enableColor)
{
    setColorMode(enableColor);
    Game game;
    if (!game.init())
    {
        showErrorMessage(game.getError());
        return;
    }

    game.run();
}
