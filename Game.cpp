#include "Game.h"

namespace {
constexpr float BOARD_Y = 50.0f;
constexpr float CELL_SIZE = 80.0f;
constexpr float FALL_SPEED = 1500.0f;
constexpr float AI_DELAY = 0.45f;
}

Game::Game()
    : state_(GameState::MainMenu),
    scoreManager_("scores.txt"),
      currentPlayer_('X'),
      moveCount_(0),
      hoveredCol_(-1),
      vsAI_(false),
      aiDelayTimer_(0.0f),
      difficulty_(Difficulty::Easy),
      winnerInfo_{},
      fallingPiece_{}
{
    scoreManager_.load();
}

void Game::startNewGame(Difficulty diff, bool vsAI) {
    board_ = Board(); // reset board
    winnerInfo_ = WinnerInfo{};
    fallingPiece_ = FallingPiece{};
    currentPlayer_ = 'X';
    moveCount_ = 0;
    hoveredCol_ = -1;
    vsAI_ = vsAI;
    difficulty_ = diff;
    aiDelayTimer_ = 0.0f;

    if (vsAI_) {
        ai_ = createAIPlayer(diff);
        state_ = GameState::CpuGame;
    } else {
        ai_.reset();
        state_ = GameState::PvpGame;
    }
}

void Game::placePieceAnimated(int col, char player) {
    int targetRow = -1;
    for (int row = ROWS - 1; row >= 0; --row) {
        if (board_.getCell(row, col) == ' ') {
            targetRow = row;
            break;
        }
    }
    if (targetRow < 0) return;

    fallingPiece_.col = col;
    fallingPiece_.targetRow = targetRow;
    fallingPiece_.piece = player;
    fallingPiece_.y = BOARD_Y - CELL_SIZE;
    fallingPiece_.active = true;
}

void Game::processInput(int clickedCol) {
    if (state_ != GameState::PvpGame && state_ != GameState::CpuGame) return;
    if (fallingPiece_.active) return;
    if (winnerInfo_.hasWinner() || moveCount_ >= ROWS * COLS) return;
    if (vsAI_ && currentPlayer_ == 'O') return;
    if (clickedCol < 0 || clickedCol >= COLS) return;
    if (!board_.isColumnPlayable(clickedCol)) return;

    placePieceAnimated(clickedCol, currentPlayer_);
}

void Game::advanceAnimation(float deltaTime) {
    if (!fallingPiece_.active) return;

    fallingPiece_.y += FALL_SPEED * deltaTime;
    float targetY = BOARD_Y + fallingPiece_.targetRow * CELL_SIZE + CELL_SIZE / 2.0f;
    if (fallingPiece_.y >= targetY) {
        fallingPiece_.y = targetY;
        board_.setCell(fallingPiece_.targetRow, fallingPiece_.col, fallingPiece_.piece);
        moveCount_++;

        winnerInfo_ = board_.checkWinner(fallingPiece_.piece);
        if (winnerInfo_.hasWinner()) {
            if (vsAI_ && fallingPiece_.piece == 'X') {
                scoreManager_.saveIfPlayerWon(difficulty_, moveCount_, winnerInfo_.winner);
            }
        } else {
            currentPlayer_ = (fallingPiece_.piece == 'X') ? 'O' : 'X';
        }
        fallingPiece_.active = false;
    }
}

void Game::runAILogic(float deltaTime) {
    if (!vsAI_ || state_ != GameState::CpuGame) return;
    if (!ai_ || currentPlayer_ != 'O') return;
    if (fallingPiece_.active) return;
    if (winnerInfo_.hasWinner() || moveCount_ >= ROWS * COLS) return;

    aiDelayTimer_ += deltaTime;
    if (aiDelayTimer_ < AI_DELAY) {
        return;
    }

    int aiCol = ai_->chooseColumn(board_, 'O', 'X');
    if (aiCol >= 0 && board_.isColumnPlayable(aiCol)) {
        placePieceAnimated(aiCol, 'O');
    }
    aiDelayTimer_ = 0.0f;
}

void Game::update(float deltaTime) {
    advanceAnimation(deltaTime);
    if (vsAI_ && state_ == GameState::CpuGame) {
        runAILogic(deltaTime);
    }
}

void Game::setState(GameState state) {
    state_ = state;
}

void Game::setHoveredCol(int col) {
    hoveredCol_ = col;
}

GameState Game::getState() const {
    return state_;
}

const Board& Game::getBoard() const {
    return board_;
}

const FallingPiece& Game::getFallingPiece() const {
    return fallingPiece_;
}

const WinnerInfo& Game::getWinnerInfo() const {
    return winnerInfo_;
}

char Game::getCurrentPlayer() const {
    return currentPlayer_;
}

int Game::getMoveCount() const {
    return moveCount_;
}

int Game::getHoveredCol() const {
    return hoveredCol_;
}

bool Game::isVsAI() const {
    return vsAI_;
}

Difficulty Game::getDifficulty() const {
    return difficulty_;
}

const ScoreManager& Game::getScoreManager() const {
    return scoreManager_;
}