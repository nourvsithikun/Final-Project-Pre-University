#pragma once

#include "raylib.h"

class Laser {
public:
    Laser(Vector2 start, Vector2 end, float lifetime = 0.14F);

    void Update(float deltaTime);
    void Draw() const;
    bool IsAlive() const { return remainingLifetime_ > 0.0F; }

private:
    Vector2 start_;
    Vector2 end_;
    float lifetime_;
    float remainingLifetime_;
};

