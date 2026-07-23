#include "Enemy.h"
#include <algorithm>
#include <array>

namespace {
constexpr float ShipWidth = 60.0F;
constexpr float ShipHeight = 60.0F;
constexpr float WordFontSize = 22.0F;
constexpr std::array<Color, 5> EnemyColors{{
    {70, 155, 255, 255},
    {245, 70, 75, 255},
    {90, 225, 120, 255},
    {185, 95, 245, 255},
    {255, 190, 55, 255}
}};
}

void Enemy::Draw(Font font, const Texture2D* texture) const {
    if (!active_) {
        return;
    }

    const Color shipColor = EnemyColors[static_cast<std::size_t>(variant_)];
    if (texture != nullptr) {
        const Rectangle source{0.0F, 0.0F, static_cast<float>(texture->width), static_cast<float>(texture->height)};
        const Rectangle destination{position_.x, position_.y, ShipWidth, ShipHeight};
        DrawTexturePro(*texture, source, destination, {ShipWidth / 2.0F, ShipHeight / 2.0F}, 0.0F, WHITE);
    } else {
        DrawCircleGradient(static_cast<int>(position_.x), static_cast<int>(position_.y + 30.0F),
                           11.0F, Color{90, 220, 255, 220}, BLANK);
        DrawTriangle(
            {position_.x - 10.0F, position_.y - 8.0F},
            {position_.x - 31.0F, position_.y + 18.0F},
            {position_.x - 7.0F, position_.y + 13.0F},
            Fade(shipColor, 0.92F)
        );
        DrawTriangle(
            {position_.x + 10.0F, position_.y - 8.0F},
            {position_.x + 7.0F, position_.y + 13.0F},
            {position_.x + 31.0F, position_.y + 18.0F},
            Fade(shipColor, 0.92F)
        );
        DrawTriangle({position_.x, position_.y + 29.0F},
                     {position_.x - 13.0F, position_.y - 18.0F},
                     {position_.x + 13.0F, position_.y - 18.0F}, shipColor);
        DrawTriangleLines({position_.x, position_.y + 29.0F},
                          {position_.x - 13.0F, position_.y - 18.0F},
                          {position_.x + 13.0F, position_.y - 18.0F}, Fade(WHITE, 0.88F));
        DrawCircleV({position_.x, position_.y - 2.0F}, 7.0F, Color{210, 236, 248, 255});
        DrawCircleV({position_.x, position_.y - 2.0F}, 3.0F, Fade(shipColor, 0.9F));
        DrawLineEx({position_.x - 28.0F, position_.y + 17.0F},
                   {position_.x - 10.0F, position_.y + 6.0F}, 2.0F, Fade(WHITE, 0.6F));
        DrawLineEx({position_.x + 28.0F, position_.y + 17.0F},
                   {position_.x + 10.0F, position_.y + 6.0F}, 2.0F, Fade(WHITE, 0.6F));
    }

    const Vector2 wordSize = MeasureTextEx(font, word_.c_str(), WordFontSize, 0.4F);
    const float boxWidth = std::max(142.0F, wordSize.x + 32.0F);
    const Rectangle wordBox{position_.x - boxWidth / 2.0F, position_.y - 75.0F, boxWidth, 45.0F};
    DrawRectangleRounded({wordBox.x + 3.0F, wordBox.y + 4.0F, wordBox.width, wordBox.height},
                         0.16F, 7, Fade(BLACK, 0.62F));
    DrawRectangleRounded(wordBox, 0.16F, 7, Color{6, 12, 23, 242});
    DrawRectangleRoundedLinesEx(wordBox, 0.16F, 7, targeted_ ? 3.0F : 2.0F,
                                targeted_ ? Color{255, 196, 65, 255} : Color{155, 111, 52, 255});
    DrawRectangleRoundedLinesEx({wordBox.x + 4.0F, wordBox.y + 4.0F, wordBox.width - 8.0F, wordBox.height - 8.0F},
                                0.16F, 7, 1.0F, targeted_ ? Fade(Color{255, 225, 143, 255}, 0.55F)
                                                                 : Fade(Color{108, 144, 183, 255}, 0.45F));

    float textX = position_.x - wordSize.x / 2.0F;
    const float textY = wordBox.y + 9.0F;
    for (std::size_t index = 0; index < word_.size(); ++index) {
        const char characterText[2]{word_[index], '\0'};
        const Color color = index < typedProgress_ ? Color{115, 244, 91, 255} : Color{247, 245, 237, 255};
        DrawTextEx(font, characterText, {textX + 1.0F, textY + 1.0F}, WordFontSize, 0.4F, Fade(BLACK, 0.9F));
        DrawTextEx(font, characterText, {textX, textY}, WordFontSize, 0.4F, color);
        textX += MeasureTextEx(font, characterText, WordFontSize, 0.4F).x;
    }

    const float progressWidth = wordBox.width - 24.0F;
    DrawRectangle(static_cast<int>(wordBox.x + 12.0F), static_cast<int>(wordBox.y + wordBox.height + 4.0F),
                  static_cast<int>(progressWidth), 4, Color{3, 9, 15, 220});
    if (!word_.empty()) {
        const float completed = progressWidth * static_cast<float>(typedProgress_) / static_cast<float>(word_.size());
        DrawRectangle(static_cast<int>(wordBox.x + 12.0F), static_cast<int>(wordBox.y + wordBox.height + 4.0F),
                      static_cast<int>(completed), 4, Color{101, 230, 72, 255});
    }

    if (targeted_ && !IsComplete()) {
        const float pulse = 3.0F + 1.5F * static_cast<float>((GetTime() * 5.0) - static_cast<int>(GetTime() * 5.0));
        DrawCircleLines(static_cast<int>(position_.x), static_cast<int>(position_.y), 36.0F + pulse,
                        Fade(Color{255, 193, 62, 255}, 0.8F));
    }
}
