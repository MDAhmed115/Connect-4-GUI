#pragma once
#include <memory>
#include "Board.h"

enum class Difficulty { Easy, Medium, Hard };

class AIPlayer {
public:
    explicit AIPlayer(Difficulty difficulty);
    virtual ~AIPlayer() = default;

    virtual int chooseColumn(const Board& board, char cpuPiece, char humanPiece) const = 0;

    Difficulty getDifficulty() const;

private:
    Difficulty difficulty_;

protected:
    static int easyMove (const Board& board);
    static int mediumMove(const Board& board, char cpu, char human);
    static int hardMove (const Board& board, char cpu, char human);
};

class EasyAI final : public AIPlayer {
public:
    EasyAI();
    int chooseColumn(const Board& board, char cpuPiece, char humanPiece) const override;
};

class MediumAI final : public AIPlayer {
public:
    MediumAI();
    int chooseColumn(const Board& board, char cpuPiece, char humanPiece) const override;
};

class HardAI final : public AIPlayer {
public:
    HardAI();
    int chooseColumn(const Board& board, char cpuPiece, char humanPiece) const override;
};

std::unique_ptr<AIPlayer> createAIPlayer(Difficulty difficulty);
