#pragma once
#include <memory>
#include "Board.h"
#include "AIPlayer.h"
#include "ScoreManager.h"
#include "FallingPiece.h"
#include "WinnerInfo.h"

enum class GameState {
    MainMenu, CpuMenu, PvpGame, CpuGame,
    ShowRules, ShowScores, EndScreen, ExitGame
};

class Game {
public:
    Game();

    void startNewGame(Difficulty diff, bool vsAI);
    void processInput(int clickedCol);   // called by Renderer on mouse click
    void update(float deltaTime);        // advances animation, AI delay timer

    // --- read-only accessors for Renderer ---
    GameState          getState()         const;
    const Board&       getBoard()         const;
    const FallingPiece& getFallingPiece() const;
    const WinnerInfo&  getWinnerInfo()    const;
    char               getCurrentPlayer() const;
    int                getMoveCount()     const;
    int                getHoveredCol()    const;
    bool               isVsAI()          const;
    Difficulty         getDifficulty()    const;
    const ScoreManager& getScoreManager() const;

    // --- state transitions (called by Renderer button callbacks) ---
    void setState(GameState state);
    void setHoveredCol(int col);

private:
    GameState   state_;
    Board       board_;
    WinnerInfo  winnerInfo_;
    FallingPiece fallingPiece_;
    ScoreManager scoreManager_;

    std::unique_ptr<AIPlayer> ai_;   // nullptr in PvP mode

    char  currentPlayer_;
    int   moveCount_;
    int   hoveredCol_;
    bool  vsAI_;
    float aiDelayTimer_;
    Difficulty difficulty_;

    void placePieceAnimated(int col, char piece);
    void advanceAnimation(float deltaTime);
    void runAILogic(float deltaTime);
};