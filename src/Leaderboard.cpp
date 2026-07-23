#include "Leaderboard.h"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <utility>

using json = nlohmann::json;

Leaderboard::Leaderboard(std::string filePath) : filePath_(std::move(filePath)) {}

bool Leaderboard::Load() {
    entries_.clear();
    std::ifstream input(filePath_);
    if (!input) {
        std::cerr << "Warning: leaderboard file not found; a new one will be created.\n";
        return false;
    }

    try {
        json document;
        input >> document;
        const json* values = nullptr;
        if (document.is_array()) {
            values = &document;
        } else if (document.is_object() && document.contains("entries") && document["entries"].is_array()) {
            values = &document["entries"];
        }
        if (values == nullptr) {
            throw json::type_error::create(302, "leaderboard root must contain an entries array", &document);
        }

        for (const auto& item : *values) {
            if (!item.is_object()) {
                continue;
            }
            LeaderboardEntry entry;
            entry.playerName = item.value("playerName", item.value("name", std::string{}));
            entry.score = item.value("score", -1);
            entry.wpm = item.value("wpm", -1.0);
            entry.accuracy = item.value("accuracy", -1.0);
            entry.wordsDestroyed = item.value("wordsDestroyed", -1);
            if (IsValid(entry)) {
                entries_.push_back(std::move(entry));
            }
        }
        SortAndTrim();
        return true;
    } catch (const std::exception& error) {
        entries_.clear();
        std::cerr << "Warning: leaderboard JSON is corrupted (" << error.what()
                  << "); continuing with an empty leaderboard.\n";
        return false;
    }
}

bool Leaderboard::Save() const {
    try {
        const std::filesystem::path path(filePath_);
        if (path.has_parent_path()) {
            std::filesystem::create_directories(path.parent_path());
        }
        json values = json::array();
        for (const auto& entry : entries_) {
            values.push_back({
                {"playerName", entry.playerName},
                {"score", entry.score},
                {"wpm", entry.wpm},
                {"accuracy", entry.accuracy},
                {"wordsDestroyed", entry.wordsDestroyed}
            });
        }
        std::ofstream output(filePath_, std::ios::trunc);
        if (!output) {
            return false;
        }
        output << json{{"entries", values}}.dump(2) << '\n';
        return output.good();
    } catch (const std::exception& error) {
        std::cerr << "Warning: could not save leaderboard: " << error.what() << '\n';
        return false;
    }
}

bool Leaderboard::AddEntry(const LeaderboardEntry& entry) {
    if (!IsValid(entry)) {
        return false;
    }
    entries_.push_back(entry);
    SortAndTrim();
    return Save();
}

bool Leaderboard::IsValid(const LeaderboardEntry& entry) {
    return !entry.playerName.empty() && entry.playerName.size() <= 20 && entry.score >= 0
        && entry.wordsDestroyed >= 0 && std::isfinite(entry.wpm) && entry.wpm >= 0.0
        && std::isfinite(entry.accuracy) && entry.accuracy >= 0.0 && entry.accuracy <= 100.0;
}

void Leaderboard::SortAndTrim() {
    std::stable_sort(entries_.begin(), entries_.end(), [](const auto& left, const auto& right) {
        if (left.score != right.score) {
            return left.score > right.score;
        }
        return left.wpm > right.wpm;
    });
    if (entries_.size() > 10) {
        entries_.resize(10);
    }
}

