#pragma once
#include "Types.h"
#include "GameEngine/Engine/Configuration.h"

namespace GameEngine
{
struct ShadowsBuffer
{
    mat4 directionalLightSpace[Params::MAX_SHADOW_MAP_CASADES];
    vec4 cascadesDistance;
    float cascadesSize;
};
}  // namespace GameEngine
