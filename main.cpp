#include <iostream>
#include <stdexcept>
#include "console.h"
#include "Menu.h"

using std::cerr;
using std::endl;

int main()
{
    init_console();

    try {
        Menu menu;
        menu.run();
    }
    catch (const std::exception& e)
    {
        cleanup_console();
        cerr << "Exception caught in main: " << e.what() << endl;
        return 1;
    }
    catch (...) {
        cleanup_console();
        cerr << "Unknown exception caught in main." << endl;
        return 1;
    }

    cleanup_console();
    return 0;
}
