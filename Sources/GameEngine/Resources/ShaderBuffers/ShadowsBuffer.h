#pragma once
#include "Types.h"
#include "GameEngine/Engine/Configuration.h"

namespace GameEngine
{
struct ShadowsBuffer
{
    AlignWrapper<mat4> directionalLightSpace[Params::MAX_SHADOW_MAP_CASADES];
    AlignWrapper<vec4> cascadesDistance;
    AlignWrapper<float> cascadesSize;
};
}  // namespace GameEngine
