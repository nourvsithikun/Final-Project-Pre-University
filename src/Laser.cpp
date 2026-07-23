#include "Laser.h"

#include <algorithm>

Laser::Laser(Vector2 start, Vector2 end, float lifetime)
    : start_(start), end_(end), lifetime_(lifetime), remainingLifetime_(lifetime) {}

void Laser::Update(float deltaTime) {
    remainingLifetime_ -= deltaTime;
}

void Laser::Draw() const {
    if (!IsAlive()) {
        return;
    }
    const float alpha = std::clamp(remainingLifetime_ / lifetime_, 0.0F, 1.0F);
    BeginBlendMode(BLEND_ADDITIVE);
    DrawLineEx(start_, end_, 8.0F, Fade(Color{30, 122, 255, 255}, alpha * 0.25F));
    DrawLineEx(start_, end_, 4.0F, Fade(Color{35, 195, 255, 255}, alpha * 0.7F));
    DrawLineEx(start_, end_, 1.5F, Fade(WHITE, alpha));
    EndBlendMode();
}

