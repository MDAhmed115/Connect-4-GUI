#pragma once
#include <vector>
#include <string>
#include "AIPlayer.h"  // for Difficulty

struct ScoreEntry {
    Difficulty  difficulty;
    int         moves;
    int         score;
};

class ScoreManager {
public:
    explicit ScoreManager(const std::string& filePath);

    void load();
    void saveIfPlayerWon(Difficulty diff, int moves, int winner);

    const std::vector<ScoreEntry>& getAll() const;

    int computeScore(Difficulty diff, int moves) const;

private:
    std::string              filePath_;
    std::vector<ScoreEntry>  scores_;

    static constexpr int MAX_SCORES = 20;
};