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

inline std::ostream& operator<<(std::ostream& os, const PerTerrainTexturesBuffer& p)
{
    os << "rgbaTextureScales: " << p.rgbaTextureScales.value << "\n";
    os << "backgroundTextureScales: " << p.backgroundTextureScales.value << "\n";
    os << "haveTextureR: " << p.haveTextureR.value << "\n";
    os << "haveTextureG: " << p.haveTextureG.value << "\n";
    os << "haveTextureB: " << p.haveTextureB.value << "\n";
    os << "haveTextureA: " << p.haveTextureA.value << "\n";
    os << "haveTextureRock: " << p.haveTextureRock.value << "\n";
    os << "haveTextureBackground: " << p.haveTextureBackground.value << "\n";
    os << "haveBlendMap: " << p.haveBlendMap.value << "\n";
    return os;
}
}  // namespace GameEngine