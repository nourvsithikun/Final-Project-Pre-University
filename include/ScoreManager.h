#pragma once

class ScoreManager {
public:
    void Reset();
    void UpdateActiveTime(float deltaTime);
    void RecordCorrectCharacter();
    void RecordWrongCharacter();
    void RecordCompletedWord(int wordLength);
    void RecordMissedEnemy();

    int GetScore() const { return score_; }
    int GetCombo() const { return combo_; }
    int GetHighestCombo() const { return highestCombo_; }
    int GetCorrectCharacters() const { return correctCharacters_; }
    int GetWrongCharacters() const { return wrongCharacters_; }
    int GetTotalCharacters() const { return correctCharacters_ + wrongCharacters_; }
    int GetWordsDestroyed() const { return wordsDestroyed_; }
    int GetLevel() const { return wordsDestroyed_ / 10 + 1; }
    float GetActiveSeconds() const { return activeSeconds_; }
    double GetAccuracy() const;
    double GetWpm() const;

private:
    int score_ = 0;
    int combo_ = 0;
    int highestCombo_ = 0;
    int correctCharacters_ = 0;
    int wrongCharacters_ = 0;
    int wordsDestroyed_ = 0;
    float activeSeconds_ = 0.0F;
};

