#pragma once
#include <Types.h>

namespace GameEngine
{
struct PerTerrainTexturesBuffer
{
    AlignWrapper<vec4> rgbaTextureScales;
    AlignWrapper<vec4> backgroundTextureScales;
    AlignWrapper<vec4> haveTextureR;
    AlignWrapper<vec4> haveTextureG;
    AlignWrapper<vec4> haveTextureB;
    AlignWrapper<vec4> haveTextureA;
    AlignWrapper<vec4> haveTextureRock;
    AlignWrapper<vec4> haveTextureBackground;
    AlignWrapper<float> haveBlendMap;

    PerTerrainTexturesBuffer()
    {
        rgbaTextureScales       = vec4(1.f);
        backgroundTextureScales = vec4(1.f);
        haveTextureR            = vec4(0.f);
        haveTextureG            = vec4(0.f);
        haveTextureB            = vec4(0.f);
        haveTextureA            = vec4(0.f);
        haveTextureRock         = vec4(0.f);
        haveTextureBackground   = vec4(0.f);
        haveBlendMap            = 0.f;
    }
};
}  // namespace GameEngine