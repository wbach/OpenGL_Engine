#pragma once
#include "Types.h"

namespace GameEngine
{
struct PerMeshObject
{
    AlignWrapper<vec4> ambient;
    AlignWrapper<vec4> diffuse;
    AlignWrapper<vec4> specular;
    AlignWrapper<uint32> numberOfRows;
    AlignWrapper<float> haveDiffTexture;
    AlignWrapper<float> haveNormalMap;
    AlignWrapper<float> haveSpecularMap;
    AlignWrapper<float> shineDamper;
    AlignWrapper<float> useFakeLighting;
};
}  // namespace GameEngine
