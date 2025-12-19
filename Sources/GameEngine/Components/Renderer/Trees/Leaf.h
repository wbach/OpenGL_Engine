#pragma once
#include <Types.h>

namespace GameEngine
{
struct Leaf
{
    vec3 position;
    vec3 direction;
    int textureIndex;
    vec3 colorRandomness;
    float sizeRandomness;
};
}  // namespace GameEngine
