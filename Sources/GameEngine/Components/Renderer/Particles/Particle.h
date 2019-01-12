#pragma once
#include "Types.h"

namespace GameEngine
{
struct Particle
{
    float scale         = 1.f;
    float rotation      = 0.f;
    float lifeTime      = 1.f;
    float gravityEffect = 0.f;
    float elapsedTime   = 0.f;
    vec3 position       = vec3(0);
    vec3 velocity       = vec3(0);

    bool update(float deltaTime, float gravity)
    {
        velocity.y += gravity * gravityEffect * deltaTime;
        auto change = velocity;
        change *= deltaTime;
        position += change;
        elapsedTime += deltaTime;
        return elapsedTime < lifeTime;
    }
};
}  // namespace GameEngine
