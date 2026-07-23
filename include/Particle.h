#pragma once

#include "raylib.h"

class Particle {
public:
    Particle(Vector2 position, Vector2 velocity, float lifetime, float size, Color color);

    void Update(float deltaTime);
    void Draw() const;
    bool IsAlive() const { return remainingLifetime_ > 0.0F; }

private:
    Vector2 position_;
    Vector2 velocity_;
    float lifetime_;
    float remainingLifetime_;
    float size_;
    Color color_;
};

