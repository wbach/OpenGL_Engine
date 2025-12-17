#include "PerMeshObject.h"

#include "GameEngine/Resources/Models/Material.h"
#include "GameEngine/Resources/Textures/GeneralTexture.h"

namespace GameEngine
{
PerMeshObject createPerMeshObject(const Material& material)
{
    PerMeshObject perMeshObject;

    perMeshObject.baseColor.value = material.baseColor;
    perMeshObject.params.value.x = material.metallicFactor;
    perMeshObject.params.value.y = material.roughnessFactor;
    perMeshObject.params.value.z = material.ambientOcclusion;
    perMeshObject.params.value.w = material.opacityCutoff;

    perMeshObject.params2.value.x = material.normalScale;
    perMeshObject.params2.value.y = material.useFakeLighting ? 1.f : 0.f;
    perMeshObject.params2.value.z = material.reflectivity;
    perMeshObject.params2.value.w = material.indexOfRefraction;

    perMeshObject.hasTextures.value.x = material.baseColorTexture != nullptr ? 1.f : 0.f;
    perMeshObject.hasTextures.value.y = material.normalTexture != nullptr ? 1.f : 0.f;
    perMeshObject.hasTextures.value.z = material.roughnessTexture != nullptr ? 1.f : 0.f;
    perMeshObject.hasTextures.value.w = material.metallicTexture != nullptr ? 1.f : 0.f;

    perMeshObject.hasTextures2.value.x = material.ambientOcclusionTexture != nullptr ? 1.f : 0.f;
    perMeshObject.hasTextures2.value.y = material.opacityTexture != nullptr ? 1.f : 0.f;
    perMeshObject.hasTextures2.value.z = material.displacementTexture != nullptr ? 1.f : 0.f;
    perMeshObject.hasTextures2.value.w = material.tiledScale;

    return perMeshObject;
}

}  // namespace GameEngine