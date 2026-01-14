#pragma once
#include <Types.h>

#include <filesystem>

namespace GameEngine
{
class ITextureLoader;
struct Material;

struct Leaf
{
    vec3 position;
    vec3 direction;
    int textureIndex;
    vec3 colorRandomness;
    float sizeRandomness;
};

struct Billboard
{
    int resolution;
    std::vector<float> alpha;

    vec3 center;
    vec3 normal;
    vec3 tangent;
    vec3 bitangent;

    float extentU;
    float extentV;
};

struct LeafSSBO
{
    AlignWrapper<vec4> positionAndSizeRandomness;
    AlignWrapper<vec4> direction;  // w not used
    AlignWrapper<vec4> colorRandomnessAndTextureIndex;
};

LeafSSBO ConvertToSSBO(const Leaf&);
std::vector<LeafSSBO> PrepareSSBOData(const std::vector<Leaf>&);
void ExportLeafSSBO(const std::vector<LeafSSBO>&, const Material&, const std::filesystem::path&);
std::pair<std::vector<LeafSSBO>, Material> ImportLeafSSBO(ITextureLoader&, const std::filesystem::path&);
}  // namespace GameEngine
