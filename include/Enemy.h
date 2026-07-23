#pragma once

#include "raylib.h"

#include <cstddef>
#include <cstdint>
#include <string>

class Enemy {
public:
    Enemy(std::uint64_t id, std::string word, Vector2 position, float speed, int variant);

    void Update(float deltaTime);
    void Draw(Font font, const Texture2D* texture) const;

    std::uint64_t GetId() const { return id_; }
    const std::string& GetWord() const { return word_; }
    Vector2 GetPosition() const { return position_; }
    float GetSpeed() const { return speed_; }
    std::size_t GetTypedProgress() const { return typedProgress_; }
    bool IsActive() const { return active_; }
    bool IsTargeted() const { return targeted_; }
    int GetVariant() const { return variant_; }
    char GetNextCharacter() const;

    void AdvanceProgress();
    void SetTargeted(bool targeted) { targeted_ = targeted; }
    void Deactivate() { active_ = false; }
    bool IsComplete() const { return typedProgress_ >= word_.size(); }

private:
    std::uint64_t id_;
    std::string word_;
    Vector2 position_;
    float speed_;
    std::size_t typedProgress_ = 0;
    bool active_ = true;
    bool targeted_ = false;
    int variant_ = 0;
};

