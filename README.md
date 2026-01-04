# 🎮 Cooperative Puzzle Adventure

A two-player cooperative puzzle game built in C++ where teamwork is essential. Navigate through challenging rooms, solve riddles, and overcome obstacles together.

![C++](https://img.shields.io/badge/C++-17-blue?logo=cplusplus)
![Platform](https://img.shields.io/badge/Platform-Windows%20|%20macOS%20|%20Linux-lightgrey)
![License](https://img.shields.io/badge/License-MIT-green)

## 🎯 Overview

In this puzzle adventure, two players must coordinate their movements to overcome obstacles, solve riddles, and escape a series of challenging rooms. Success requires teamwork—you're not competing against each other, but working together to reach the goal.

## ✨ Features

### Core Gameplay
- **Two-Player Co-op** — Both players must work together to solve puzzles and progress
- **Multi-Level Progression** — Navigate through multiple rooms using a door-based level system
- **Lives & Revival System** — Players can sacrifice score points to revive their fallen teammate

### Game Elements
| Symbol | Element | Description |
|:------:|---------|-------------|
| `#` | Wall | Blocks movement |
| `k` | Key | Collect to open doors |
| `1-9` | Door | Requires a key to pass |
| `S` | Spring | Launches players (speed = compression time) |
| `*` | Obstacle | Heavy block—push it alone or together |
| `?` | Riddle | Answer correctly to remove it |
| `T` | Torch | Reveals hidden walls in a radius |
| `b` | Bomb | Pick up and drop to destroy walls/obstacles |
| `-/=` | Switch | Toggle to activate map elements |

### Technical Highlights
- **Object-Oriented Architecture** — Clean separation with dedicated classes for each game entity
- **Cross-Platform Support** — Runs on Windows, macOS, and Linux with platform-specific abstractions
- **Physics Engine** — Spring acceleration and combined-force obstacle pushing
- **Dynamic Lighting** — Torch system reveals hidden walls within a radius
- **Interactive Riddles** — Loaded from external files with scoring system
- **Timer-Based Explosives** — Bomb pickup/drop system with countdown

## 🎮 Controls

Movement is continuous—press a key to change direction, press STAY to stop.

| Action | Player 1 (`$`) | Player 2 (`&`) |
|--------|:--------------:|:--------------:|
| Move Up | `W` | `I` |
| Move Down | `X` | `M` |
| Move Left | `A` | `J` |
| Move Right | `D` | `L` |
| Stay/Stop | `S` | `K` |
| Drop Item | `E` | `O` |
| Revive Teammate | `R` | `R` |

**General Controls:**
- `ESC` — Pause/Resume game
- `H` — Return to main menu (while paused)

## 🏗️ Architecture

The project follows strict OOP principles with emphasis on encapsulation and single responsibility:

```
├── Game.cpp/h        # Main game loop, level management
├── Player.cpp/h      # Player state, movement, item handling
├── Screen.cpp/h      # Map loading, rendering, object management
├── Point.cpp/h       # 2D coordinate system with direction
├── Legend.cpp/h      # HUD display (score, lives, timer)
│
├── Spring.cpp/h      # Spring mechanics (compression, launch)
├── Obstacle.cpp/h    # Pushable block physics
├── Door.cpp/h        # Door state, key requirements
├── Bomb.cpp/h        # Explosion timer and radius
├── Riddle.cpp/h      # Question loading and validation
├── Switch.cpp/h      # Toggle mechanism for doors
├── Key.cpp/h         # Collectible key items
├── Torch.cpp/h       # Dynamic lighting system
│
├── Menu.cpp/h        # Main menu interface
├── console.h         # Cross-platform terminal abstraction (Windows/macOS/Linux)
├── utils.cpp/h       # Sound and helper functions
└── Constants.h       # Game-wide constants
```

## 🔧 Building

### Prerequisites
- C++17 compatible compiler
- Terminal with ANSI escape code support (most modern terminals)

### Windows (Visual Studio)
1. Open `Project1.sln`
2. Build solution (`Ctrl+Shift+B`)
3. Run (`F5`)

### Windows (Command Line - MSVC)
```bash
cl /EHsc /std:c++17 *.cpp /Fe:game.exe
```

### macOS / Linux
```bash
g++ -std=c++17 -o game *.cpp
./game
```

Or with Clang:
```bash
clang++ -std=c++17 -o game *.cpp
./game
```

## 📁 Level Files

Level files are stored in `Data/` with the naming convention `adv-world_XX.screen.txt`. The game automatically loads and sorts all matching files.

### Creating Custom Levels
Create a text file following the format with map symbols. Mark the legend position with `L`. Riddles are stored separately in `Data/riddles.txt`.

## 🎓 Learning Outcomes

This project demonstrates:
- Clean OOP design with proper encapsulation
- Cross-platform development with preprocessor-based abstractions
- Game loop architecture and state management
- File I/O for level and riddle loading
- Physics simulation (springs, collisions)
- Multi-entity interaction systems

## 📄 License

MIT License — feel free to use, modify, and learn from this code.

---

*Built as part of a C++ programming course, demonstrating practical application of object-oriented design principles.*
