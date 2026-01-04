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

struct LeafCluster
{
    vec3 position;
    vec3 direction;
    float radius;

    vec3 colorRandomness;
    float sizeRandomness;

    uint32_t seed;
    uint32_t variant;
};
}  // namespace GameEngine
