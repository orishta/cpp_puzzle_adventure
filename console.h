#pragma once

#include <iostream>
// File utilities
#include <vector>
#include <string>
#include <algorithm>
#include <thread>
#include <random>

// Auto-detect platform
#if !defined(PLATFORM_WINDOWS) && !defined(PLATFORM_UNIX)
#if defined(_WIN32) || defined(_WIN64) || defined(__WINDOWS__)
#define PLATFORM_WINDOWS
#else
#define PLATFORM_UNIX
#endif
#endif

// Includes
#ifdef PLATFORM_WINDOWS
#include <windows.h>
#include <conio.h>
#else
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/select.h> // Required for reliable kbhit on Mac
#include <sys/ioctl.h>
#include <cstdlib>
#include <cstdio>
#endif

// Color Enum
enum class Color { Black, Blue, Green, Cyan, Red, Magenta, Brown, LightGrey, DarkGrey, LightBlue, LightGreen, LightCyan, LightRed, LightMagenta, Yellow, White };

// --- File utilities: find available screen files (adv-world*.screen) ---
// Implemented inline so it's header-only and available everywhere that includes console.h
#ifdef PLATFORM_WINDOWS
// Windows already includes <windows.h> above
#else
#include <dirent.h>
#include <cstring>
#endif

inline std::vector<std::string> findScreenFiles() {
    std::vector<std::string> files;
#ifdef PLATFORM_WINDOWS
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA("Data\\adv-world*.screen*", &findData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                files.push_back(std::string("Data\\") + findData.cFileName);
            }
        } while (FindNextFileA(hFind, &findData));
        FindClose(hFind);
    }
#else
    DIR* dir = opendir("Data");
    if (dir != nullptr) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            std::string name = entry->d_name;
            // Match files starting with "adv-world" and ending with ".screen" or ".screen.txt"
            if (name.find("adv-world") == 0 &&
                ((name.length() > 7 && name.substr(name.length() - 7) == ".screen") ||
                 (name.length() > 11 && name.substr(name.length() - 11) == ".screen.txt"))) {
                files.push_back("Data/" + name);
            }
        }
        closedir(dir);
    }
#endif
    std::sort(files.begin(), files.end());
    return files;
}

// Unix/Mac Terminal Management
// Cross-platform terminal handling - AI-assisted implementation 
#ifdef PLATFORM_UNIX
static struct termios orig_termios;
static bool terminal_initialized = false;

inline void restore_terminal() {
    if (terminal_initialized) {
        tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
        terminal_initialized = false;
    }
}

inline void init_terminal() {
    if (!terminal_initialized) {
        tcgetattr(STDIN_FILENO, &orig_termios);
        atexit(restore_terminal);

        struct termios new_termios = orig_termios;
        new_termios.c_lflag &= ~(ICANON | ECHO); // Turn off line buffering and echo
        new_termios.c_cc[VMIN] = 1;
        new_termios.c_cc[VTIME] = 0;

        tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
        terminal_initialized = true;
    }
}

// Robust kbhit for Mac using select() - AI-assisted implementation 
// This checks if a key is waiting in the buffer WITHOUT blocking the game
inline bool check_kbhit() {
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0;
}

// Read one char immediately
inline int get_single_char() {
    return getchar();
}

#else 
// Windows implementations...
inline bool check_kbhit() { return _kbhit(); }
inline int get_single_char() { return _getch(); }
inline void init_terminal() {} // Not needed on Windows
inline void restore_terminal() {}
#endif

// --- Cross Platform Functions ---
// Terminal control functions - AI-assisted implementation 

inline void clrscr() {
#ifdef PLATFORM_WINDOWS
    system("cls");
#else
    std::cout << "\033[2J\033[3J\033[H" << std::flush;
#endif
}

inline void hideCursor() {
#ifdef PLATFORM_WINDOWS
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &info);
#else
    std::cout << "\033[?25l" << std::flush;
#endif
}

inline void gotoxy(int x, int y) {
#ifdef PLATFORM_WINDOWS
    std::cout.flush();
    HANDLE hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD dwCursorPosition = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hConsoleOutput, dwCursorPosition);
#else
    std::cout << "\033[" << (y + 1) << ";" << (x + 1) << "H" << std::flush;
#endif
}

inline void sleep_ms(int milliseconds) {
#ifdef PLATFORM_WINDOWS
    Sleep(milliseconds);
#else
    usleep(milliseconds * 1000);
#endif
}

// Cross-platform color system - AI-assisted implementation 
inline void set_color(Color color) {
#ifdef PLATFORM_WINDOWS
    int color_code = 15;
    switch (color) {
    case Color::Black: color_code = 0; break;
    case Color::Blue: color_code = 1; break;
    case Color::Green: color_code = 2; break;
    case Color::Cyan: color_code = 3; break;
    case Color::Red: color_code = 4; break;
    case Color::Magenta: color_code = 5; break;
    case Color::Brown: color_code = 6; break;
    case Color::LightGrey: color_code = 7; break;
    case Color::DarkGrey: color_code = 8; break;
    case Color::LightBlue: color_code = 9; break;
    case Color::LightGreen: color_code = 10; break;
    case Color::LightCyan: color_code = 11; break;
    case Color::LightRed: color_code = 12; break;
    case Color::LightMagenta: color_code = 13; break;
    case Color::Yellow: color_code = 14; break;
    case Color::White: color_code = 15; break;
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)color_code);
#else
    static const char* ansi_colors[] = {
        "\033[30m", "\033[34m", "\033[32m", "\033[36m", "\033[31m", "\033[35m", "\033[33m", "\033[37m",
        "\033[90m", "\033[94m", "\033[92m", "\033[96m", "\033[91m", "\033[95m", "\033[93m", "\033[97m"
    };
    std::cout << ansi_colors[static_cast<int>(color)];
#endif
}

// Wrappers needed for main.cpp
inline void init_console() {
#ifdef PLATFORM_UNIX
    init_terminal();
#endif
}

inline void cleanup_console() {
#ifdef PLATFORM_UNIX
    restore_terminal();
#endif
}

// Sound system - AI-assisted implementation
// Simple cross-platform audio using terminal bell character
static bool g_soundEnabled = false;

inline void setSoundEnabled(bool enabled) {
    g_soundEnabled = enabled;
}

inline bool isSoundEnabled() {
    return g_soundEnabled;
}

// Simple cross-platform beep - works on both Windows and Mac
// Uses terminal bell character '\a' - no external files needed
inline void playSound(const std::string& eventName) {
    if (!g_soundEnabled) return;

    // Terminal bell works on all platforms without blocking
    // This is the simplest solution that doesn't require binary files
    std::cout << '\a' << std::flush;
}

// Legacy C-string version for backwards compatibility
inline void playSound(const char* type) {
    playSound(std::string(type));
}

// Show a centered error message box and wait for a key press (cross-platform)
inline void showErrorMessage(const std::string& message) {
    clrscr();
    int boxWidth = (int)message.length() + 4;
    int startX = (80 - boxWidth) / 2;
    int startY = 10;

    set_color(Color::LightRed);
    gotoxy(startX, startY);
    for (int i = 0; i < boxWidth; ++i) std::cout << '=';

    gotoxy(startX, startY + 1);
    std::cout << "| " << message << " |";

    gotoxy(startX, startY + 2);
    for (int i = 0; i < boxWidth; ++i) std::cout << '=';

    set_color(Color::White);
    gotoxy(startX, startY + 4);
    std::cout << "Press any key to continue...";

    // Wait for key
    while (!check_kbhit()) { sleep_ms(50); }
    get_single_char();
}