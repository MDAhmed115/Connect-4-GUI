#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <stdbool.h>

#define ROWS 6
#define COLS 7

extern char board[ROWS][COLS];

typedef struct{
    int winner;
    int startrow;
    int endrow;
    int startcol;
    int endcol;
} WinnerInfo;

void initBoard();
int addPiece(int col, char player);
WinnerInfo checkWinner(char player);
int cpuMoveEasy(char cpu, char player);
int cpuMoveMedium(char cpu, char player);
int cpuMoveHard(char cpu, char player);

#endif
