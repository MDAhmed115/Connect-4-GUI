#pragma once

struct WinnerInfo {
    int winner;     // 0 = none, 1 = X, 2 = O
    int startRow, endRow;
    int startCol, endCol;

    bool hasWinner() const;
};