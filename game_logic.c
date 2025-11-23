#include "game_logic.h"
#include <stdlib.h>
#include <time.h>

char board[ROWS][COLS];

void initBoard(){
    int i, j;
    srand((unsigned)time(NULL));
    for (i = 0; i < ROWS; i++)
    {
        for (j = 0; j < COLS; j++)
        {
            board[i][j] = ' ';
        }
    }
}

int addPiece(int col, char player){
    if (col < 0 || col >= COLS) return -1;
    int i;
    for (i = ROWS - 1; i >= 0; i--)
    {
        if (board[i][col] == ' ')
        {
            board[i][col] = player;
            return i;
        }
    }
    return -1;
}

WinnerInfo checkWinner(char player){
    WinnerInfo info = {0, -1, -1, -1, -1};
    int win = (player == 'X') ? 1 : 2;
    int r, c;

    for (r = 0; r < ROWS; r++){
        for (c = 0; c <= COLS - 4; c++){
            if (board[r][c] == player && board[r][c+1] == player && board[r][c+2] == player && board[r][c+3] == player){
                info.winner = win;
                info.startrow = r;
                info.endrow = r;
                info.startcol = c;
                info.endcol = c+3;
                return info;
            }
        }
    }

    for (r = 0; r <= ROWS - 4; r++){
        for (c = 0; c < COLS; c++){
            if (board[r][c] == player && board[r+1][c] == player && board[r+2][c] == player && board[r+3][c] == player){
                info.winner = win;
                info.startrow = r;
                info.endrow = r+3;
                info.startcol = c;
                info.endcol = c;
                return info;
            }
        }
    }

    for (r = 0; r <= ROWS - 4; r++){
        for (c = 0; c <= COLS - 4; c++){
            if (board[r][c] == player && board[r+1][c+1] == player && board[r+2][c+2] == player && board[r+3][c+3] == player){
                info.winner = win;
                info.startrow = r;
                info.endrow = r+3;
                info.startcol = c;
                info.endcol = c+3;
                return info;
            }
        }
    }

    for (r = 0; r <= ROWS - 4; r++){
        for (c = 3; c < COLS; c++){
            if (board[r][c] == player && board[r+1][c-1] == player && board[r+2][c-2] == player && board[r+3][c-3] == player){
                info.winner = win;
                info.startrow = r;
                info.endrow = r+3;
                info.startcol = c;
                info.endcol = c-3;
                return info;
            }
        }
    }

    return info;
}

int cpuMoveEasy(char cpu, char player){
    int col = rand() % COLS;
    while (board[0][col] != ' ') col = rand() % COLS;
    return col;
}

int cpuMoveMedium(char cpu, char player){
    int col, row;

    for (col = 0; col < COLS; col++){
        if (board[0][col] != ' ') continue;
        addPiece(col, player);
        if (checkWinner(player).winner != 0){
            for (row = 0; row < ROWS; row++){
                if (board[row][col] != ' '){
                    board[row][col] = ' ';
                    break;
                }
            }
            return col;
        }
        for (row = 0; row < ROWS; row++){
            if (board[row][col] != ' '){
                board[row][col] = ' ';
                break;
            }
        }
    }

    for (col = 0; col < COLS; col++){
        if (board[0][col] == ' ') return col;
    }

    return -1;
}

int cpuMoveHard(char cpu, char player){
    int col, row;
    for (col = 0; col < COLS; col++){
        if (board[0][col] != ' ') continue;
        addPiece(col, cpu);
        if (checkWinner(cpu).winner != 0){
            for (row = 0; row < ROWS; row++){
                if (board[row][col] != ' '){
                    board[row][col] = ' ';
                    break;
                }
            }
            return col;
        }
        for (row = 0; row < ROWS; row++){
            if (board[row][col] != ' '){
                board[row][col] = ' ';
                break;
            }
        }
    }

    for (col = 0; col < COLS; col++){
        if (board[0][col] != ' ') continue;
        addPiece(col, player);
        if (checkWinner(player).winner != 0){
            for (row = 0; row < ROWS; row++){
                if (board[row][col] != ' '){
                    board[row][col] = ' ';
                    break;
                }
            }
            return col;
        }
        for (row = 0; row < ROWS; row++){
            if (board[row][col] != ' '){
                board[row][col] = ' ';
                break;
            }
        }
    }

    int weights[COLS];
    int i;
    int baseWeights[7] = {1,2,4,8,4,2,1};
    for (i = 0; i < COLS; i++) weights[i] = baseWeights[i];
    int total = 0;
    for (i = 0; i < COLS; i++) total += weights[i];
    if (total <= 0) total = 1;
    int random = rand() % total;
    int sum = 0;
    for (col = 0; col < COLS; col++){
        sum += weights[col];
        if (random < sum && board[0][col] == ' '){
            return col;
        }
    }

    for (col = 0; col < COLS; col++){
        if (board[0][col] == ' ') return col;
    }

    return -1;
}
