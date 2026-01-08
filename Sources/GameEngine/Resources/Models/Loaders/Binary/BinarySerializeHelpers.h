#pragma once
#include <GraphicsApi/MeshRawDataSerilizeHelpers.h>
#include <bitsery/adapter/stream.h>
#include <bitsery/bitsery.h>
#include <bitsery/traits/string.h>
#include <bitsery/traits/vector.h>

#include <optional>
#include <vector>

#include "GameEngine/Resources/Models/Material.h"
#include "GameEngine/Resources/Models/Mesh.h"
#include "GameEngine/Resources/Models/Model.h"

namespace bitsery
{
template <typename S>
void serialize(S& s, glm::vec4& v)
{
    s.value4b(v.x);
    s.value4b(v.y);
    s.value4b(v.z);
    s.value4b(v.w);
}
}  // namespace bitsery

namespace bitsery
{
template <typename S>
void serialize(S& s, glm::vec3& v)
{
    s.value4b(v.x);
    s.value4b(v.y);
    s.value4b(v.z);
}
}  // namespace bitsery

namespace GameEngine
{
namespace Animation
{
template <typename S>
void serialize(S& s, Joint::AdditionalUserMofiyTransform& t)
{
    // s.object(t.offset);
    // s.object(t.transform);
    // s.object(t.finalTransform);
}

template <typename S>
void serialize(S& s, Joint& joint)
{
    // parent wskaźnik pomijamy
    s.value4b(joint.id);
    s.value4b(joint.size);
    s.text1b(joint.name);  // string

    s.object(joint.additionalUserMofiyTransform);

    s.value1b(joint.ignoreParentRotation);

    s.object(joint.transform);
    s.object(joint.offset);
    s.object(joint.invtransform);
    s.object(joint.animatedTransform);

    constexpr size_t MaxChildren = 256;
    s.container(joint.children, MaxChildren);
}
}  // namespace Animation
}  // namespace GameEngine

namespace GameEngine
{
struct MaterialTextureSerilizeData
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

    std::string diffuseTexture;
    std::string ambientTexture;
    std::string specularTexture;

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

    std::string baseColorTexture;
    std::string normalTexture;
    std::string metallicTexture;
    std::string roughnessTexture;
    std::string ambientOcclusionTexture;
    std::string opacityTexture;
    std::string displacementTexture;
};

struct MeshSerializeData
{
    GraphicsApi::RenderType renderType;
    GraphicsApi::MeshRawData meshRawData;
    MaterialTextureSerilizeData material;
    mat4 transform       = mat4(1.f);
    vec3 normalizedScale = vec3(1.f);
};

struct ModelSerializeData
{
    std::vector<MeshSerializeData> meshes_;
    std::optional<Animation::Joint> skeleton_;
    float normalizedFactor{1.f};
};

template <typename S>
void serialize(S& s, MaterialTextureSerilizeData& m)
{
    constexpr size_t MaxStr = 256;

    s.text1b(m.name, MaxStr);

    s.object(m.ambient);
    s.object(m.diffuse);
    s.object(m.specular);

    s.value4b(m.shineDamper);
    s.value4b(m.reflectivity);
    s.value4b(m.indexOfRefraction);

    s.value1b(m.isTransparency);
    s.value1b(m.useFakeLighting);

    s.text1b(m.diffuseTexture, MaxStr);
    s.text1b(m.ambientTexture, MaxStr);
    s.text1b(m.specularTexture, MaxStr);

    s.object(m.baseColor);
    s.value4b(m.metallicFactor);
    s.value4b(m.roughnessFactor);
    s.value4b(m.ambientOcclusion);

    s.value4b(m.tiledScale);
    s.value4b(m.normalScale);
    s.value4b(m.opacityCutoff);
    s.value4b(m.subsurfaceStrength);
    s.object(m.subsurfaceColor);

    s.text1b(m.baseColorTexture, MaxStr);
    s.text1b(m.normalTexture, MaxStr);
    s.text1b(m.metallicTexture, MaxStr);
    s.text1b(m.roughnessTexture, MaxStr);
    s.text1b(m.ambientOcclusionTexture, MaxStr);
    s.text1b(m.opacityTexture, MaxStr);
    s.text1b(m.displacementTexture, MaxStr);
}

template <typename S>
void serialize(S& s, MeshSerializeData& meshData)
{
    using U = std::underlying_type_t<GraphicsApi::RenderType>;
    s.value1b(reinterpret_cast<U&>(meshData.renderType));
    s.object(meshData.meshRawData);
    s.object(meshData.material);
}

template <typename S>
void serialize(S& s, ModelSerializeData& modelData)
{
    constexpr size_t MaxMeshes = 1024;
    s.container(modelData.meshes_, MaxMeshes);

    // s.object(modelData.skeleton_);
    s.value4b(modelData.normalizedFactor);
}
}  // namespace GameEngine
