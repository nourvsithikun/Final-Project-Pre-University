#include "Enemy.h"

#include <utility>

Enemy::Enemy(std::uint64_t id, std::string word, Vector2 position, float speed, int variant)
    : id_(id), word_(std::move(word)), position_(position), speed_(speed), variant_(variant % 5) {}

void Enemy::Update(float deltaTime) {
    if (active_) {
        position_.y += speed_ * deltaTime;
    }
}

char Enemy::GetNextCharacter() const {
    return IsComplete() ? '\0' : word_[typedProgress_];
}

void Enemy::AdvanceProgress() {
    if (!IsComplete()) {
        ++typedProgress_;
    }
}

