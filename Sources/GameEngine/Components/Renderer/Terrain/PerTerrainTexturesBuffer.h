#pragma once
#include <Types.h>

namespace GameEngine
{
struct PerTerrainTexturesBuffer
{
    AlignWrapper<vec4> rgbaTextureScales;
    AlignWrapper<vec4> backgroundTextureScales;
};
}  // namespace GameEngine