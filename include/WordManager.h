#pragma once

#include <random>
#include <string>
#include <vector>

class WordManager {
public:
    WordManager();

    bool LoadAll(const std::string& dataDirectory);
    std::string GetRandomWord(int level);
    void ResetLevelPool();

    const std::vector<std::string>& GetEasyWords() const { return easyWords_; }
    const std::vector<std::string>& GetMediumWords() const { return mediumWords_; }
    const std::vector<std::string>& GetHardWords() const { return hardWords_; }

private:
    static bool LoadFile(const std::string& path, std::vector<std::string>& output);
    static std::string Normalize(const std::string& value);
    void InstallFallbacksIfNeeded();
    void PreparePoolForLevel(int level);

    std::vector<std::string> easyWords_;
    std::vector<std::string> mediumWords_;
    std::vector<std::string> hardWords_;
    std::vector<std::string> levelPool_;
    std::size_t nextWordIndex_ = 0;
    int poolLevel_ = 0;
    std::mt19937 random_;
};
