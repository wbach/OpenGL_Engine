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

struct LeafSSBO
{
    AlignWrapper<vec4> positionAndSizeRandomness;
    AlignWrapper<vec4> direction;// w not used
    AlignWrapper<vec4> colorRandomnessAndTextureIndex;
};

LeafSSBO ConvertToSSBO(const Leaf&);
std::vector<LeafSSBO> PrepareSSBOData(const std::vector<Leaf>&);
}  // namespace GameEngine
