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

    // Phong legacy
    vec4 ambient{vec4(0.f, 0.f, 0.f, 1.f)};
    vec4 diffuse{vec4(0.8f, 0.8f, 0.8f, 1.f)};
    vec4 specular{vec4(0.f, 0.f, 0.f, 1.f)};

    float shineDamper       = 0.f;
    float reflectivity      = 0.f;
    float indexOfRefraction = 0.f;

    bool isTransparency  = false;
    bool useFakeLighting = false;

    TextureSerilizeData diffuseTexture;
    TextureSerilizeData ambientTexture;
    TextureSerilizeData specularTexture;

    // PBR – Physically Based Rendering
    vec4 baseColor{1.f, 1.f, 1.f, 1.f};
    float metallicFactor   = 0.0f;
    float roughnessFactor  = 0.7f;
    float ambientOcclusion = 1.0f;

    float tiledScale         = 1.0f;
    float normalScale        = 1.0f;
    float opacityCutoff      = 0.5f;
    float subsurfaceStrength = 0.0f;
    vec3 subsurfaceColor{0.0f};

    TextureSerilizeData baseColorTexture;
    TextureSerilizeData normalTexture;
    TextureSerilizeData metallicTexture;
    TextureSerilizeData roughnessTexture;
    TextureSerilizeData ambientOcclusionTexture;
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
