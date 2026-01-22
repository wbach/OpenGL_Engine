#pragma once
#include <GraphicsApi/MeshRawDataSerilizeHelpers.h>
#include <Logger/Log.h>
#include <Utils/Image/ImageSerializer.h>
#include <bitsery/adapter/stream.h>
#include <bitsery/bitsery.h>
#include <bitsery/ext/std_optional.h>
#include <bitsery/traits/string.h>
#include <bitsery/traits/vector.h>

#include <magic_enum/magic_enum.hpp>
#include <optional>
#include <vector>

#include "ModelReleatedSerializationData.h"

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
    s.value4b(joint.id);
    s.value4b(joint.size);
    s.text1b(joint.name);

    s.object(joint.additionalUserMofiyTransform);

    s.value1b(joint.ignoreParentRotation);

    s.object(joint.transform);
    s.object(joint.offset);
    s.object(joint.invtransform);
    s.object(joint.animatedTransform);

    constexpr size_t MaxChildren = 2048;
    s.container(joint.children, MaxChildren);
}
}  // namespace Animation
}  // namespace GameEngine

namespace GameEngine
{
template <typename S>
void serialize(S& s, TextureParameters& parameters)
{
    auto serialize_enum = [&s](auto& e)
    {
        using T      = std::decay_t<decltype(e)>;
        uint32_t tmp = magic_enum::enum_integer(e);
        s.value4b(tmp);
        e = magic_enum::enum_cast<T>(tmp).value();
    };

    serialize_enum(parameters.flipMode);
    serialize_enum(parameters.loadType);
    serialize_enum(parameters.sizeLimitPolicy);
    serialize_enum(parameters.dataStorePolicy);
    serialize_enum(parameters.filter);
    serialize_enum(parameters.mimap);
}

template <typename S>
void serialize(S& s, TextureSerilizeData& m)
{
    constexpr size_t MaxStr = 2048;
    s.text1b(m.path, MaxStr);
    s.object(m.paramters);
    s.ext(m.image, bitsery::ext::StdOptional{});
}

template <typename S>
void serialize(S& s, MaterialSerilizeData& m)
{
    constexpr size_t MaxStr = 2048;

    s.text1b(m.name, MaxStr);

    s.object(m.ambient);
    s.object(m.diffuse);
    s.object(m.specular);

    s.value4b(m.shineDamper);
    s.value4b(m.reflectivity);
    s.value4b(m.indexOfRefraction);

    s.value1b(m.isTransparency);
    s.value1b(m.useFakeLighting);

    s.object(m.diffuseTexture);
    s.object(m.ambientTexture);
    s.object(m.specularTexture);

    s.object(m.baseColor);
    s.value4b(m.metallicFactor);
    s.value4b(m.roughnessFactor);
    s.value4b(m.ambientOcclusion);

    s.value4b(m.tiledScale);
    s.value4b(m.normalScale);
    s.value4b(m.opacityCutoff);
    s.value4b(m.subsurfaceStrength);
    s.object(m.subsurfaceColor);

    s.object(m.baseColorTexture);
    s.object(m.normalTexture);
    s.object(m.metallicTexture);
    s.object(m.roughnessTexture);
    s.object(m.ambientOcclusionTexture);
    s.object(m.opacityTexture);
    s.object(m.displacementTexture);

    s.value4b(m.flags);
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
    constexpr size_t MaxMeshes = 4096;
    s.container(modelData.meshes_, MaxMeshes);

    // s.object(modelData.skeleton_);
    s.value4b(modelData.normalizedFactor);
}
}  // namespace GameEngine
