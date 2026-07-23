#include "Particle.h"

#include <algorithm>

Particle::Particle(Vector2 position, Vector2 velocity, float lifetime, float size, Color color)
    : position_(position), velocity_(velocity), lifetime_(lifetime), remainingLifetime_(lifetime),
      size_(size), color_(color) {}

void Particle::Update(float deltaTime) {
    remainingLifetime_ -= deltaTime;
    position_.x += velocity_.x * deltaTime;
    position_.y += velocity_.y * deltaTime;
    velocity_.x *= 0.985F;
    velocity_.y = velocity_.y * 0.985F + 18.0F * deltaTime;
}

void Particle::Draw() const {
    if (!IsAlive()) {
        return;
    }
    const float alpha = std::clamp(remainingLifetime_ / lifetime_, 0.0F, 1.0F);
    DrawCircleV(position_, size_ * (0.5F + alpha * 0.5F), Fade(color_, alpha));
}

