#include "Board.h"
using namespace std;

void Board::reset(){
    for (auto& row : grid_) { // empty the board by filling all cells with space character
        row.fill(' ');
    }
}
Board::Board(){
    srand((unsigned)time(NULL));
    reset();
}

int Board::addPiece(int col, char player){
    if (col < 0 || col >= COLS) return -1; // Invalid column
    for (int row = ROWS - 1; row >= 0; --row) {
        if (grid_[row][col] == ' ') {
            grid_[row][col] = player;
            return row; // Return the row where the piece was placed
        }
    }
    return -1; // Column is full
}

bool Board::isFull() const{
    for (int i = 0; i < COLS; i++){
        if (grid_[0][i] == ' ') return false; //checking top row for empty cell, if found return false
    }
    return true;
}

bool Board::isColumnPlayable(int col) const{
    if (col < 0 || col >= COLS) return false; // Invalid column
    return grid_[0][col] == ' '; // Check if top cell is empty
}

WinnerInfo Board::checkWinner(char player) const{
    WinnerInfo info = {0, -1, -1, -1, -1};
    int win = (player == 'X') ? 1 : 2;
    int r, c;

    for (r = 0; r < ROWS; r++){
        for (c = 0; c <= COLS - 4; c++){
            if (grid_[r][c] == player && grid_[r][c+1] == player && grid_[r][c+2] == player && grid_[r][c+3] == player){
                info.winner = win;
                info.startRow = r;
                info.endRow = r;
                info.startCol = c;
                info.endCol = c+3;
                return info;
            }
        }
    }

    for (r = 0; r <= ROWS - 4; r++){
        for (c = 0; c < COLS; c++){
            if (grid_[r][c] == player && grid_[r+1][c] == player && grid_[r+2][c] == player && grid_[r+3][c] == player){
                info.winner = win;
                info.startRow = r;
                info.endRow = r+3;
                info.startCol = c;
                info.endCol = c;
                return info;
            }
        }
    }

    for (r = 0; r <= ROWS - 4; r++){
        for (c = 0; c <= COLS - 4; c++){
            if (grid_[r][c] == player && grid_[r+1][c+1] == player && grid_[r+2][c+2] == player && grid_[r+3][c+3] == player){
                info.winner = win;
                info.startRow = r;
                info.endRow = r+3;
                info.startCol = c;
                info.endCol = c+3;
                return info;
            }
        }
    }

    for (r = 0; r <= ROWS - 4; r++){
        for (c = 3; c < COLS; c++){
            if (grid_[r][c] == player && grid_[r+1][c-1] == player && grid_[r+2][c-2] == player && grid_[r+3][c-3] == player){
                info.winner = win;
                info.startRow = r;
                info.endRow = r+3;
                info.startCol = c;
                info.endCol = c-3;
                return info;
            }
        }
    }

    return info;
}

char Board::getCell(int row, int col) const{
    if (row < 0 || row >= ROWS || col < 0 || col >= COLS) return ' '; // Invalid cell
    return grid_[row][col];
}

void Board::setCell(int row, int col, char value){
    grid_[row][col] = value;
}