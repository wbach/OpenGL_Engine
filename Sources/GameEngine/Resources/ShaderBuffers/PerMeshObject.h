#pragma once
#include "Types.h"

namespace GameEngine
{
struct Material;

struct PerMeshObject
{
    AlignWrapper<vec4> baseColor;
    AlignWrapper<vec4> params;        // x - metallicFactor, y - roughnessFactor, z - ambientOcclusion, w - opacityCutoff
    AlignWrapper<vec4> params2;       // x - normalScale,  y - useFakeLighting, z - reflectivity, w - indexOfRefraction
    AlignWrapper<vec4> hasTextures;   // x - BaseColorTexture, y - NormalTexture, z -RoughnessTexture, w - MetallicTexture
    AlignWrapper<vec4> hasTextures2;  // x - AmbientOcclusionTexture, y - OpacityTexture, z -DisplacementTexture, w - not used
};

PerMeshObject createPerMeshObject(const Material&);
}  // namespace GameEngine
