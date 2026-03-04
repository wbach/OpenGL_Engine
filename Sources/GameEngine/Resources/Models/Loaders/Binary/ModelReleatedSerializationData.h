#pragma once

#include <Types.h>

#include "GameEngine/Resources/Models/Material.h"
#include "GameEngine/Resources/Models/Mesh.h"
#include "GameEngine/Resources/Models/Model.h"
#include "GameEngine/Resources/TextureParameters.h"

namespace GameEngine
{
struct TextureSerilizeData
{
    TextureParameters paramters;
    std::string path;
    std::optional<Utils::Image> image;
};
struct MaterialSerilizeData
{
    std::string name{"noName"};

    vec4 baseColor{1.f, 1.f, 1.f, 1.f};
    float metallicFactor    = 0.0f;
    float roughnessFactor   = 0.7f;
    float normalScale       = 1.0f;
    float occlusionStrength = 1.0f;

    float indexOfRefraction = 1.0f;
    float transmission      = 0.0f;
    float opacityCutoff     = 0.5f;

    vec3 subsurfaceColor{0.0f};
    float subsurfaceStrength = 0.0f;

    float uvScale = 1.0f;

    TextureSerilizeData baseColorTexture;
    TextureSerilizeData normalTexture;
    TextureSerilizeData metallicTexture;
    TextureSerilizeData roughnessTexture;
    TextureSerilizeData emissionTexture;
    TextureSerilizeData occlusionTexture;
    TextureSerilizeData opacityTexture;
    TextureSerilizeData displacementTexture;

    uint32_t flags = 0;
};

struct MeshSerializeData
{
    GraphicsApi::RenderType renderType;
    GraphicsApi::MeshRawData meshRawData;
    MaterialSerilizeData material;
    mat4 transform       = mat4(1.f);
    vec3 normalizedScale = vec3(1.f);
};

struct ModelSerializeData
{
    std::vector<MeshSerializeData> meshes_;
    std::optional<Animation::Joint> skeleton_;
    float normalizedFactor{1.f};
};
}  // namespace GameEngine
