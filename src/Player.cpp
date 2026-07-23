#include "Player.h"

#include <algorithm>
#include <cmath>

Player::Player(Vector2 position) : position_(position) {}

void Player::Reset() {
    health_ = 3;
    enginePulse_ = 0.0F;
    hitFlashTimer_ = 0.0F;
}

void Player::Damage() {
    health_ = std::max(0, health_ - 1);
    hitFlashTimer_ = 0.35F;
}

void Player::Update(float deltaTime) {
    enginePulse_ += deltaTime * 8.0F;
    hitFlashTimer_ = std::max(0.0F, hitFlashTimer_ - deltaTime);
}

void Player::Draw(const Texture2D* texture) const {
    const Color tint = hitFlashTimer_ > 0.0F ? Color{255, 130, 130, 255} : WHITE;
    if (texture != nullptr) {
        const Rectangle source{0.0F, 0.0F, static_cast<float>(texture->width), static_cast<float>(texture->height)};
        const Rectangle destination{position_.x, position_.y, 55.0F, 65.0F};
        DrawTexturePro(*texture, source, destination, {27.5F, 32.5F}, 0.0F, tint);
    } else {
        const Color body = hitFlashTimer_ > 0.0F ? RED : Color{61, 157, 239, 255};
        DrawTriangle({position_.x - 9.0F, position_.y + 13.0F},
                     {position_.x - 31.0F, position_.y + 28.0F},
                     {position_.x - 12.0F, position_.y - 4.0F}, Fade(body, 0.9F));
        DrawTriangle({position_.x + 9.0F, position_.y + 13.0F},
                     {position_.x + 12.0F, position_.y - 4.0F},
                     {position_.x + 31.0F, position_.y + 28.0F}, Fade(body, 0.9F));
        DrawTriangle({position_.x, position_.y - 36.0F},
                     {position_.x - 15.0F, position_.y + 28.0F},
                     {position_.x + 15.0F, position_.y + 28.0F}, body);
        DrawTriangleLines({position_.x, position_.y - 36.0F},
                          {position_.x - 15.0F, position_.y + 28.0F},
                          {position_.x + 15.0F, position_.y + 28.0F}, Fade(WHITE, 0.9F));
        DrawLineEx({position_.x - 28.0F, position_.y + 25.0F},
                   {position_.x - 10.0F, position_.y + 12.0F}, 2.0F, Fade(WHITE, 0.6F));
        DrawLineEx({position_.x + 28.0F, position_.y + 25.0F},
                   {position_.x + 10.0F, position_.y + 12.0F}, 2.0F, Fade(WHITE, 0.6F));
        DrawCircleV({position_.x, position_.y - 2.0F}, 8.0F, Color{228, 246, 255, 255});
        DrawCircleV({position_.x, position_.y - 2.0F}, 4.0F, Color{53, 138, 221, 255});
    }

    const float glow = 11.0F + std::sin(enginePulse_) * 4.0F;
    DrawCircleGradient(static_cast<int>(position_.x), static_cast<int>(position_.y + 38.0F), glow,
                       Color{74, 220, 255, 210}, BLANK);
}
