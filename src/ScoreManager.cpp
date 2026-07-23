#include "ScoreManager.h"

#include <algorithm>

void ScoreManager::Reset() {
    score_ = 0;
    combo_ = 0;
    highestCombo_ = 0;
    correctCharacters_ = 0;
    wrongCharacters_ = 0;
    wordsDestroyed_ = 0;
    activeSeconds_ = 0.0F;
}

void ScoreManager::UpdateActiveTime(float deltaTime) {
    activeSeconds_ += std::max(0.0F, deltaTime);
}

void ScoreManager::RecordCorrectCharacter() {
    ++correctCharacters_;
}

void ScoreManager::RecordWrongCharacter() {
    ++wrongCharacters_;
    combo_ = 0;
    score_ = std::max(0, score_ - 2);
}

void ScoreManager::RecordCompletedWord(int wordLength) {
    ++wordsDestroyed_;
    ++combo_;
    highestCombo_ = std::max(highestCombo_, combo_);
    score_ += std::max(0, wordLength) * 10 + combo_ * 2;
}

void ScoreManager::RecordMissedEnemy() {
    combo_ = 0;
    score_ = std::max(0, score_ - 20);
}

double ScoreManager::GetAccuracy() const {
    const int total = GetTotalCharacters();
    if (total == 0) {
        return 100.0;
    }
    return static_cast<double>(correctCharacters_) / static_cast<double>(total) * 100.0;
}

double ScoreManager::GetWpm() const {
    if (activeSeconds_ <= 0.0F) {
        return 0.0;
    }
    const double minutes = static_cast<double>(activeSeconds_) / 60.0;
    return (static_cast<double>(correctCharacters_) / 5.0) / minutes;
}

