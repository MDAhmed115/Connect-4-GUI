#pragma once
#include <array>
#include "WinnerInfo.h"
#include <stdlib.h>
#include <time.h>

constexpr int ROWS = 6;
constexpr int COLS = 7;

class Board {
public:
    Board();

    void reset();
    int addPiece(int col, char player);   // returns row placed, -1 if invalid
    bool isFull() const;
    bool isColumnPlayable(int col) const;

    WinnerInfo checkWinner(char player) const;

    char getCell(int row, int col) const; //getter for a specific cell
    void setCell(int row, int col, char value); //setter for a specific cell


private:
    std::array<std::array<char, COLS>, ROWS> grid_;
};