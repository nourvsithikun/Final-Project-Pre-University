#pragma once

#include <string>
#include <vector>

struct LeaderboardEntry {
    std::string playerName;
    int score = 0;
    double wpm = 0.0;
    double accuracy = 0.0;
    int wordsDestroyed = 0;
};

class Leaderboard {
public:
    explicit Leaderboard(std::string filePath = {});

    bool Load();
    bool Save() const;
    bool AddEntry(const LeaderboardEntry& entry);
    void SetFilePath(const std::string& filePath) { filePath_ = filePath; }
    const std::vector<LeaderboardEntry>& GetEntries() const { return entries_; }

private:
    static bool IsValid(const LeaderboardEntry& entry);
    void SortAndTrim();

    std::string filePath_;
    std::vector<LeaderboardEntry> entries_;
};
