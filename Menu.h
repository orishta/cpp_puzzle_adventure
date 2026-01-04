#pragma once

class Menu {
public:
    void run();

private:
    void displayMainMenu();
    void handleChoice(char choice);
    void printInstructions();
    void launchGame(bool enableColor);
};
