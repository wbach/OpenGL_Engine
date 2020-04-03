#pragma once
#include "Types.h"

namespace GameEngine
{
struct Plane
{
    vec3 normal_; // a b c
    float d_;  // The "distance" to the origin.

    void Normalize()
    {
        float scale = 1.f / glm::length(normal_);
        normal_.x *= scale;
        normal_.y *= scale;
        normal_.z *= scale;
        d_ *= scale;
    }
};
}  // namespace GameEngine
