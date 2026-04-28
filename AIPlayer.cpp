#include "AIPlayer.h"

#include <cstdlib>
#include <limits>
#include <vector>

namespace {
void removeSimulatedPiece(Board& board, int col) {
    for (int row = 0; row < ROWS; ++row) {
        if (board.getCell(row, col) != ' ') {
            board.setCell(row, col, ' ');
            break;
        }
    }
}

std::vector<int> getPlayableColumns(const Board& board) {
    std::vector<int> cols;
    cols.reserve(COLS);
    for (int col = 0; col < COLS; ++col) {
        if (board.getCell(0, col) == ' ') cols.push_back(col);
    }
    return cols;
}

bool isWinningMove(Board& board, int col, char piece) {
    if (board.getCell(0, col) != ' ') return false;
    board.addPiece(col, piece);
    bool wins = (board.checkWinner(piece).winner != 0);
    removeSimulatedPiece(board, col);
    return wins;
}

int scoreWindow(const Board& board, int r, int c, int dr, int dc, char cpu, char human) {
    int cpuCount = 0;
    int humanCount = 0;
    int emptyCount = 0;

    for (int i = 0; i < 4; ++i) {
        const char cell = board.getCell(r + i * dr, c + i * dc);
        if (cell == cpu) cpuCount++;
        else if (cell == human) humanCount++;
        else emptyCount++;
    }

    if (cpuCount > 0 && humanCount > 0) return 0;

    if (cpuCount == 4) return 100000;
    if (humanCount == 4) return -100000;
    if (cpuCount == 3 && emptyCount == 1) return 120;
    if (humanCount == 3 && emptyCount == 1) return -140;
    if (cpuCount == 2 && emptyCount == 2) return 16;
    if (humanCount == 2 && emptyCount == 2) return -18;
    if (cpuCount == 1 && emptyCount == 3) return 2;
    if (humanCount == 1 && emptyCount == 3) return -2;
    return 0;
}

int evaluateBoard(const Board& board, char cpu, char human) {
    int score = 0;

    // Center control is valuable in Connect 4.
    for (int row = 0; row < ROWS; ++row) {
        const char centerCell = board.getCell(row, COLS / 2);
        if (centerCell == cpu) score += 6;
        else if (centerCell == human) score -= 6;
    }

    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c <= COLS - 4; ++c) {
            score += scoreWindow(board, r, c, 0, 1, cpu, human);
        }
    }
    for (int r = 0; r <= ROWS - 4; ++r) {
        for (int c = 0; c < COLS; ++c) {
            score += scoreWindow(board, r, c, 1, 0, cpu, human);
        }
    }
    for (int r = 0; r <= ROWS - 4; ++r) {
        for (int c = 0; c <= COLS - 4; ++c) {
            score += scoreWindow(board, r, c, 1, 1, cpu, human);
        }
    }
    for (int r = 0; r <= ROWS - 4; ++r) {
        for (int c = 3; c < COLS; ++c) {
            score += scoreWindow(board, r, c, 1, -1, cpu, human);
        }
    }

    return score;
}

int minimax(Board& board, int depth, bool maximizing, char cpu, char human, int alpha, int beta) {
    if (board.checkWinner(cpu).winner != 0) return 1000000 + depth;
    if (board.checkWinner(human).winner != 0) return -1000000 - depth;

    const std::vector<int> playable = getPlayableColumns(board);
    if (depth == 0 || playable.empty()) {
        return evaluateBoard(board, cpu, human);
    }

    if (maximizing) {
        int best = std::numeric_limits<int>::min();
        for (int col : playable) {
            board.addPiece(col, cpu);
            int score = minimax(board, depth - 1, false, cpu, human, alpha, beta);
            removeSimulatedPiece(board, col);

            if (score > best) best = score;
            if (best > alpha) alpha = best;
            if (beta <= alpha) break;
        }
        return best;
    }

    int best = std::numeric_limits<int>::max();
    for (int col : playable) {
        board.addPiece(col, human);
        int score = minimax(board, depth - 1, true, cpu, human, alpha, beta);
        removeSimulatedPiece(board, col);

        if (score < best) best = score;
        if (best < beta) beta = best;
        if (beta <= alpha) break;
    }
    return best;
}

bool givesOpponentImmediateWin(Board& board, int cpuCol, char cpu, char human) {
    board.addPiece(cpuCol, cpu);
    const std::vector<int> playable = getPlayableColumns(board);
    bool givesWin = false;
    for (int col : playable) {
        if (isWinningMove(board, col, human)) {
            givesWin = true;
            break;
        }
    }
    removeSimulatedPiece(board, cpuCol);
    return givesWin;
}
}

AIPlayer::AIPlayer(Difficulty diff) : difficulty_(diff) {}

Difficulty AIPlayer::getDifficulty() const {
    return difficulty_;
}

int AIPlayer::easyMove(const Board& board) {
    int col = rand() % COLS;
    while (board.getCell(0, col) != ' ') col = rand() % COLS;
    return col;
}

int AIPlayer::mediumMove(const Board& board, char cpu, char human) {
    (void)cpu;
    int col;
    Board tempBoard = board;
    for (col = 0; col < COLS; col++) {
        if (tempBoard.getCell(0, col) != ' ') continue;
        tempBoard.addPiece(col, human);
        if (tempBoard.checkWinner(human).winner != 0) {
            removeSimulatedPiece(tempBoard, col);
            return col;
        }
        removeSimulatedPiece(tempBoard, col);
    }

    // If no immediate block is needed, Medium falls back to Easy's random playable column.
    return AIPlayer::easyMove(board);
}

int AIPlayer::hardMove(const Board& board, char cpu, char human) {
    int col;
    Board tempBoard = board;

    for (col = 0; col < COLS; col++) {
        if (tempBoard.getCell(0, col) != ' ') continue;
        tempBoard.addPiece(col, cpu);
        if (tempBoard.checkWinner(cpu).winner != 0) {
            removeSimulatedPiece(tempBoard, col);
            return col;
        }
        removeSimulatedPiece(tempBoard, col);
    }

    for (col = 0; col < COLS; col++) {
        if (tempBoard.getCell(0, col) != ' ') continue;
        tempBoard.addPiece(col, human);
        if (tempBoard.checkWinner(human).winner != 0) {
            removeSimulatedPiece(tempBoard, col);
            return col;
        }
        removeSimulatedPiece(tempBoard, col);
    }

    std::vector<int> candidates = getPlayableColumns(tempBoard);
    if (candidates.empty()) return -1;

    // Avoid moves that allow an immediate winning reply when possible.
    std::vector<int> safeCandidates;
    safeCandidates.reserve(candidates.size());
    for (int candidateCol : candidates) {
        if (!givesOpponentImmediateWin(tempBoard, candidateCol, cpu, human)) {
            safeCandidates.push_back(candidateCol);
        }
    }
    if (!safeCandidates.empty()) {
        candidates = safeCandidates;
    }

    const int searchDepth = 5;
    int bestScore = std::numeric_limits<int>::min();
    std::vector<int> bestCols;

    for (int candidateCol : candidates) {
        tempBoard.addPiece(candidateCol, cpu);
        int score = minimax(tempBoard, searchDepth - 1, false, cpu, human,
                            std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
        removeSimulatedPiece(tempBoard, candidateCol);

        // Slight preference toward center when scores are close.
        score -= std::abs(candidateCol - (COLS / 2));

        if (score > bestScore) {
            bestScore = score;
            bestCols.clear();
            bestCols.push_back(candidateCol);
        } else if (score == bestScore) {
            bestCols.push_back(candidateCol);
        }
    }

    if (!bestCols.empty()) {
        return bestCols[rand() % bestCols.size()];
    }

    return AIPlayer::easyMove(board);
}

EasyAI::EasyAI() : AIPlayer(Difficulty::Easy) {}

int EasyAI::chooseColumn(const Board& board, char cpuPiece, char humanPiece) const {
    (void)cpuPiece;
    (void)humanPiece;
    return AIPlayer::easyMove(board);
}

MediumAI::MediumAI() : AIPlayer(Difficulty::Medium) {}

int MediumAI::chooseColumn(const Board& board, char cpuPiece, char humanPiece) const {
    return AIPlayer::mediumMove(board, cpuPiece, humanPiece);
}

HardAI::HardAI() : AIPlayer(Difficulty::Hard) {}

int HardAI::chooseColumn(const Board& board, char cpuPiece, char humanPiece) const {
    return AIPlayer::hardMove(board, cpuPiece, humanPiece);
}

std::unique_ptr<AIPlayer> createAIPlayer(Difficulty difficulty) {
    switch (difficulty) {
        case Difficulty::Easy:   return std::make_unique<EasyAI>();
        case Difficulty::Medium: return std::make_unique<MediumAI>();
        case Difficulty::Hard:   return std::make_unique<HardAI>();
    }
    return std::make_unique<EasyAI>();
}