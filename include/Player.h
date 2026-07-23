#pragma once

#include "raylib.h"

class Player {
public:
    explicit Player(Vector2 position = {800.0F, 720.0F});

    void Reset();
    void Damage();
    void Update(float deltaTime);
    void Draw(const Texture2D* texture) const;

    Vector2 GetPosition() const { return position_; }
    int GetHealth() const { return health_; }
    bool IsAlive() const { return health_ > 0; }

private:
    Vector2 position_;
    int health_ = 3;
    float enginePulse_ = 0.0F;
    float hitFlashTimer_ = 0.0F;
};

