#pragma once
#include <Types.h>

namespace GameEngine
{
struct SkyPassBuffer
{
    mat4 invProj;
    mat4 invViewRot;
    AlignWrapper<vec4> screenSize;
    AlignWrapper<vec4> sunDirection;
    AlignWrapper<vec4> sunColor;  // w is night
};
}  // namespace GameEngine
