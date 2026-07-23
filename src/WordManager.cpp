#include "WordManager.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>

WordManager::WordManager() : random_(std::random_device{}()) {
    InstallFallbacksIfNeeded();
}

bool WordManager::LoadAll(const std::string& dataDirectory) {
    easyWords_.clear();
    mediumWords_.clear();
    hardWords_.clear();

    const bool easyLoaded = LoadFile(dataDirectory + "/easy_words.txt", easyWords_);
    const bool mediumLoaded = LoadFile(dataDirectory + "/medium_words.txt", mediumWords_);
    const bool hardLoaded = LoadFile(dataDirectory + "/hard_words.txt", hardWords_);

    if (!easyLoaded) {
        std::cerr << "Warning: easy word file missing or empty; using fallback words.\n";
    }
    if (!mediumLoaded) {
        std::cerr << "Warning: medium word file missing or empty; using fallback words.\n";
    }
    if (!hardLoaded) {
        std::cerr << "Warning: hard word file missing or empty; using fallback words.\n";
    }

    InstallFallbacksIfNeeded();
    ResetLevelPool();
    return easyLoaded && mediumLoaded && hardLoaded;
}

std::string WordManager::GetRandomWord(int level) {
    if (poolLevel_ != level) {
        PreparePoolForLevel(level);
    }

    if (nextWordIndex_ >= levelPool_.size()) {
        return {};
    }

    return levelPool_[nextWordIndex_++];
}

void WordManager::ResetLevelPool() {
    levelPool_.clear();
    nextWordIndex_ = 0;
    poolLevel_ = 0;
}

void WordManager::PreparePoolForLevel(int level) {
    const std::vector<std::string>* words = &easyWords_;
    if (level >= 5) {
        words = &hardWords_;
    } else if (level >= 3) {
        words = &mediumWords_;
    }

    levelPool_ = *words;
    std::shuffle(levelPool_.begin(), levelPool_.end(), random_);
    nextWordIndex_ = 0;
    poolLevel_ = level;
}

bool WordManager::LoadFile(const std::string& path, std::vector<std::string>& output) {
    std::ifstream input(path);
    if (!input) {
        return false;
    }

    std::string line;
    while (std::getline(input, line)) {
        std::string word = Normalize(line);
        if (!word.empty() && std::find(output.begin(), output.end(), word) == output.end()) {
            output.push_back(std::move(word));
        }
    }
    return !output.empty();
}

std::string WordManager::Normalize(const std::string& value) {
    auto begin = value.begin();
    while (begin != value.end() && std::isspace(static_cast<unsigned char>(*begin)) != 0) {
        ++begin;
    }
    auto end = value.end();
    while (end != begin && std::isspace(static_cast<unsigned char>(*(end - 1))) != 0) {
        --end;
    }

    std::string normalized;
    normalized.reserve(static_cast<std::size_t>(std::distance(begin, end)));
    for (auto iterator = begin; iterator != end; ++iterator) {
        const unsigned char character = static_cast<unsigned char>(*iterator);
        if (std::isalpha(character) != 0) {
            normalized.push_back(static_cast<char>(std::tolower(character)));
        }
    }
    return normalized;
}

void WordManager::InstallFallbacksIfNeeded() {
    if (easyWords_.empty()) {
        easyWords_ = {"star", "moon", "khmer", "laser", "angkor", "rocket", "temple", "comet"};
    }
    if (mediumWords_.empty()) {
        mediumWords_ = {"planet", "meteor", "culture", "wonder", "galaxy", "island", "legacy", "shuttle"};
    }
    if (hardWords_.empty()) {
        hardWords_ = {"heritage", "astronaut", "satellite", "universe", "spaceship", "constellation", "civilization"};
    }
}
