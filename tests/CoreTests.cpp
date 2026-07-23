#include "Enemy.h"
#include "Leaderboard.h"
#include "ScoreManager.h"
#include "Targeting.h"
#include "WordManager.h"

#include <cassert>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace {
bool AlmostEqual(double left, double right, double tolerance = 0.001) {
    return std::abs(left - right) <= tolerance;
}

void TestScoreManager() {
    ScoreManager score;
    score.Reset();
    assert(score.GetScore() == 0);
    assert(AlmostEqual(score.GetAccuracy(), 100.0));
    for (int index = 0; index < 25; ++index) score.RecordCorrectCharacter();
    score.UpdateActiveTime(60.0F);
    assert(AlmostEqual(score.GetWpm(), 5.0));
    score.RecordWrongCharacter();
    assert(score.GetWrongCharacters() == 1);
    assert(score.GetScore() == 0);
    assert(score.GetCombo() == 0);
    score.RecordCompletedWord(5);
    assert(score.GetScore() == 52);
    assert(score.GetCombo() == 1);
    score.RecordMissedEnemy();
    assert(score.GetScore() == 32);
    assert(score.GetCombo() == 0);
    for (int index = 1; index < 10; ++index) score.RecordCompletedWord(1);
    assert(score.GetWordsDestroyed() == 10);
    assert(score.GetLevel() == 2);
}

void TestDuplicateTargeting() {
    std::vector<Enemy> enemies;
    enemies.emplace_back(10, "star", Vector2{400.0F, 250.0F}, 40.0F, 0);
    enemies.emplace_back(11, "sun", Vector2{600.0F, 510.0F}, 40.0F, 1);
    enemies.emplace_back(12, "moon", Vector2{800.0F, 600.0F}, 40.0F, 2);
    const auto selected = SelectLowestEnemyStartingWith(enemies, 'S');
    assert(selected.has_value());
    assert(*selected == 11);
    enemies[1].Deactivate();
    const auto second = SelectLowestEnemyStartingWith(enemies, 's');
    assert(second.has_value() && *second == 10);
}

void TestWordFallbackAndNormalization(const std::filesystem::path& root) {
    WordManager missing;
    assert(!missing.LoadAll((root / "missing").string()));
    assert(!missing.GetEasyWords().empty());
    assert(!missing.GetMediumWords().empty());
    assert(!missing.GetHardWords().empty());

    const auto data = root / "words";
    std::filesystem::create_directories(data);
    for (const char* filename : {"easy_words.txt", "medium_words.txt", "hard_words.txt"}) {
        std::ofstream output(data / filename);
        output << "  AnGkOr  \n\nMOON\nANGKOR\n";
    }
    WordManager loaded;
    assert(loaded.LoadAll(data.string()));
    assert(loaded.GetEasyWords().front() == "angkor");
    assert(loaded.GetEasyWords().at(1) == "moon");
    assert(loaded.GetEasyWords().size() == 2);

    const std::string first = loaded.GetRandomWord(1);
    const std::string second = loaded.GetRandomWord(1);
    assert(first != second);
    assert(loaded.GetRandomWord(1).empty());

    const std::string nextLevelFirst = loaded.GetRandomWord(2);
    const std::string nextLevelSecond = loaded.GetRandomWord(2);
    assert(nextLevelFirst != nextLevelSecond);
    assert(loaded.GetRandomWord(2).empty());
}

void TestLeaderboard(const std::filesystem::path& root) {
    const auto file = root / "leaderboard.json";
    {
        std::ofstream corrupted(file);
        corrupted << "{ this is not json";
    }
    Leaderboard board(file.string());
    assert(!board.Load());
    assert(board.GetEntries().empty());

    for (int index = 0; index < 12; ++index) {
        const LeaderboardEntry entry{"PLAYER" + std::to_string(index), index * 100, 20.0 + index,
                                     90.0, index};
        assert(board.AddEntry(entry));
    }
    assert(board.GetEntries().size() == 10);
    assert(board.GetEntries().front().score == 1100);
    assert(board.GetEntries().back().score == 200);

    Leaderboard reloaded(file.string());
    assert(reloaded.Load());
    assert(reloaded.GetEntries().size() == 10);
    assert(reloaded.GetEntries().front().playerName == "PLAYER11");
}
}

int main() {
    const std::filesystem::path root = std::filesystem::current_path() / ".typing_space_shooter_tests";
    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root);

    TestScoreManager();
    TestDuplicateTargeting();
    TestWordFallbackAndNormalization(root);
    TestLeaderboard(root);

    std::filesystem::remove_all(root);
    std::cout << "All core tests passed.\n";
    return 0;
}
