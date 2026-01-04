#include "utils.h"
#include <iostream>

static bool g_useColors = true;

void cls() {
    clrscr();
}

void setColorMode(bool enable) {
    g_useColors = enable;
}

bool isColorMode() {
    return g_useColors;
}