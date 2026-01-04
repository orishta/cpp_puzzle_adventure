#pragma once
#include "console.h"

void cls();

void setColorMode(bool enable);
bool isColorMode();


inline void setTextColor(Color c) {
    if (isColorMode()) {
        set_color(c);
    }
}

inline void setTextColor(int c) {
    setTextColor(static_cast<Color>(c));
}