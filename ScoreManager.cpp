#include "ScoreManager.h"
#include <fstream>
using namespace std;

void ScoreManager::load() {
    scores_.clear();
    ifstream file(filePath_);
    if (file.is_open()) {
        ScoreEntry entry;
        int diffValue = 0;
        while (file >> diffValue >> entry.moves >> entry.score) {
            if (diffValue < static_cast<int>(Difficulty::Easy) || diffValue > static_cast<int>(Difficulty::Hard)) {
                continue;
            }
            entry.difficulty = static_cast<Difficulty>(diffValue);
            scores_.push_back(entry);
            if (scores_.size() >= MAX_SCORES) break;
        }
        file.close();
    }
}

int ScoreManager::computeScore(Difficulty diff, int moves) const{
    int base, penalty, score;
    if (diff == Difficulty::Easy){base = 1000; penalty = 10;}
    else if (diff == Difficulty::Medium){base = 1200; penalty = 12;}
    else{base = 1500; penalty = 15;}
    score = base - (moves * penalty);
    if (score < 0) score = 0;
    return score;
}

void ScoreManager::saveIfPlayerWon(Difficulty diff, int moves, int winner){
    if (winner == 1){ // player won
        ScoreEntry newEntry{diff, moves, computeScore(diff, moves)};
        scores_.insert(scores_.begin(), newEntry);
        if (scores_.size() > MAX_SCORES) {
            scores_.pop_back();
        }
        ofstream file(filePath_, ios::trunc);
        if (file.is_open()) {
            for (const auto& entry : scores_) {
                file << static_cast<int>(entry.difficulty) << ' '
                     << entry.moves << ' '
                     << entry.score << '\n';
            }
            file.close();
        }
    }
}

const vector<ScoreEntry>& ScoreManager::getAll() const {
    return scores_;
}

ScoreManager::ScoreManager(const string& filePath) : filePath_(filePath) {
}