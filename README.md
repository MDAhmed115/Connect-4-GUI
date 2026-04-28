# Connect 4 — OOP GUI Edition

<div align="center">

A polished graphical Connect 4 game rebuilt in **C++** using **Raylib**, applying full **Object-Oriented Programming** principles — featuring smooth animations, an enhanced minimalist UI, native fullscreen support, multiple AI difficulty levels, Player vs CPU and Player vs Player modes, and a persistent score tracking system.

<br/>

![C++](https://img.shields.io/badge/Language-C%2B%2B17-blue?style=flat-square&logo=cplusplus)
![Raylib](https://img.shields.io/badge/Graphics-Raylib-orange?style=flat-square)
![Platform](https://img.shields.io/badge/Platform-Windows-informational?style=flat-square&logo=windows)
![Build](https://img.shields.io/badge/Build-GCC%20MinGW--w64-success?style=flat-square)
![Course](https://img.shields.io/badge/Course-OOP%20Spring%202026-purple?style=flat-square)

</div>

---

## Features

### Gameplay
- Classic 6×7 Connect 4 mechanics.
- Player vs Player mode.
- Player vs CPU with **Easy**, **Medium**, and **Hard** AI difficulties.
- Turn-based hover indicators showing the targeted column.
- Highlighted winning line with animation on game end.

### Visuals & UI
- Built entirely with **Raylib**.
- **Enhanced GUI** with a clean, **minimalist aesthetic** — polished piece rendering, layered glow effects, and refined color palette.
- **Native fullscreen support** — launches and scales correctly at the system's native resolution.
- Smooth **falling piece animation** with per-frame physics.
- Animated win line that grows outward from the centre of the winning sequence.
- Column hover glow feedback with fill/empty zone distinction.
- Main menu, difficulty selection, rules screen, scores screen, and end-game interface.
- Displays final score above the board after a player victory.

### Scoring System
- Score formula:

```
score = base − (moves × penalty)
```

- Difficulty-based scoring — higher difficulty awards a higher base score and applies a higher per-move penalty:

| Difficulty | Base Score | Penalty per Move |
|:----------:|:----------:|:----------------:|
| Easy       | 1000       | 10               |
| Medium     | 1200       | 12               |
| Hard       | 1500       | 15               |

- Only **Player victories in Player vs CPU mode** are recorded.
- Newest scores appear **on top**.
- Automatically retains only the **20 most recent** scores.

---

## Object-Oriented Architecture

This project was migrated from a procedural C codebase to a fully OOP C++ architecture. Every global variable has been eliminated and replaced with class ownership. The four pillars of OOP are applied throughout:

| Pillar | Where applied |
|--------|---------------|
| **Encapsulation** | `Board` owns the grid privately; all access is through typed methods |
| **Abstraction** | `Game::update()` and `Renderer::draw()` hide all internal complexity |
| **Inheritance** | `EasyAI`, `MediumAI`, `HardAI` all inherit from abstract `AIPlayer` |
| **Polymorphism** | `Game` calls `ai_->chooseColumn()` — dispatch is automatic at runtime |

### Class Overview

```
Game              — central coordinator; owns all state
├── Board         — 6×7 grid, piece placement, win detection
├── AIPlayer*     — abstract base; EasyAI / MediumAI / HardAI inherit from it
├── ScoreManager  — file I/O, score calculation, persistent storage
├── WinnerInfo    — result struct (winner flag + winning cell coordinates)
└── FallingPiece  — animation state (column, target row, current y, active flag)

Renderer          — all Raylib calls live here only; reads Game state
```

> `Renderer` is the **only** class that includes `raylib.h`. The entire logic layer is independent of any graphics framework.

---

## Technologies Used

| Component | Technology |
|-----------|-----------|
| Language | C++17 |
| Graphics Library | Raylib |
| Compiler | GCC via MSYS2 MinGW-w64 |
| Editor | Visual Studio Code |
| Build System | MinGW Make / manual g++ linking |

---

## How to Build & Run

### 1. Install Raylib (MSYS2)

```bash
pacman -S mingw-w64-x86_64-raylib
```

### 2. Compile

```bash
g++ main.cpp Game.cpp Board.cpp AIPlayer.cpp ScoreManager.cpp Renderer.cpp WinnerInfo.cpp -o connect4 -lraylib -lopengl32 -lgdi32 -lwinmm -std=c++17
```

Or use the included `build.bat`:

```bash
build.bat
```

### 3. Run

```bash
./connect4.exe
```

---

## Score File Format

Scores are saved automatically to `scores.txt`. Each line contains:

```
difficulty moves score
```

Where `difficulty` is stored as an integer: `0` = Easy, `1` = Medium, `2` = Hard.

**Example:**

```
2 11 1335
1 14 1032
0 9 910
```

---

## Flowchart

![Flowchart](https://raw.githubusercontent.com/MDAhmed115/Connect-4-GUI/main/flowchart.png)

---

## Future Enhancements

- Add sound effects and background music.
- Add visual themes and colour skins.
- Save full match history beyond the 20-score limit.
- Keyboard navigation support.
- In-game settings panel (volume, difficulty, theme).

---

## Limitations

- No keyboard support for gameplay input (mouse only).
- No networking or online multiplayer.
- Requires Raylib DLLs to be present when running on other Windows machines.

---

## Author

**Muhammed Ahmed Ashfaq**  
FAST-NUCES Karachi  
Object Oriented Programming Lab — Spring 2026
