#pragma once
#include "Types.h"

namespace GameEngine
{
struct PerFrameBuffer
{
    mat4 ProjectionViewMatrix = mat4(1.f);
    AlignWrapper<vec3> cameraPosition{vec3(0.f)};
    AlignWrapper<vec4> clipPlane{vec4(0.f, 1.f, 0.f, 100000.f)};
    AlignWrapper<vec4> projection;
    AlignWrapper<vec4> time{vec4(0.f)};
};
}  // namespace GameEngine
