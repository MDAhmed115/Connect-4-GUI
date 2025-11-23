# Connect 4 — GUI Edition
A polished graphical Connect 4 game built in **C** using **Raylib**, featuring animations, multiple difficulty levels, Player vs CPU AI, scores tracking, and a responsive user interface.

---

## Features

### Gameplay
- Classic 7×6 Connect 4 mechanics.
- Player vs Player mode.
- Player vs CPU with **Easy**, **Medium**, and **Hard** difficulties.
- Turn-based hover indicators.
- Highlighted winning line animation.

### Visuals & UI
- Built entirely with **Raylib**.
- Main menu, difficulty selection menu, and end-game interface.
- Column hover feedback.
- Displays score above the grid after a player victory.

### Scoring System
- Score formula:
score = base − (moves × penalty)

- Difficulty-based scoring:
- Higher difficulty → higher base score & higher penalties.
- Only **Player victories in PvCPU mode** are recorded.
- Newest scores appear **on top**.
- Automatically retains only **20 most recent** scores.

---

## Technologies Used
- **Language:** C  
- **Graphics Library:** Raylib  
- **Compiler:** GCC (MSYS2 MinGW-w64)  
- **Editor:** Visual Studio Code  
- **Build System:** MinGW Make / manual gcc linking  


---

## How to Build & Run

### 1. Install Raylib (MSYS2)
```bash
pacman -S mingw-w64-x86_64-raylib
```
### 2. Compile
```
gcc main.c game-logic.c -o connect4 -lraylib -lopengl32 -lgdi32 -lwinmm
```
### 3. Run
```
./connect4.exe
```

---
Score File Format (scores.txt)
Each line contains:
```
difficulty moves score
```
Example:
```
2 11 1035
1 14 860
3 9 1210
```
Flowchart:


---
### Future Enhancements
-Add sound effects.
-Improve AI using Minimax with pruning.
-Add themes / skins.
-Save full match history instead of only 20 scores.
-Add window-resize support.
-Keyboard Support

---
### Limitations:
-CPU is rule-based, not predictive.
-No Keyboard support.
-No networking or online multiplayer.
-Requires Raylib DLLs when running on other Windows machines.
---

## Author
**Muhammed Ahmed Ashfaq**
**FAST-NUCES Karachi**
**Programming Fundamentals Lab - Fall 2025**

