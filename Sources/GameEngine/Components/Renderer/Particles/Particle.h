#pragma once
#include "Types.h"

namespace GameEngine
{
struct Particle
{
    float scale         = 1.f;
    float rotation      = 0.f;
    float lifeTime      = 1.f;
    bool gravityEffect = false;
    float elapsedTime   = 0.f;
    vec3 position       = vec3(0);
    vec3 velocity       = vec3(0);

    bool update(float deltaTime, float gravity)
    {
        if (gravityEffect)
        {
            velocity.y += gravity * deltaTime;
        }

        auto change = velocity;
        change *= deltaTime;
        position += change;
        elapsedTime += deltaTime;
        return elapsedTime < lifeTime;
    }
};
}  // namespace GameEngine
