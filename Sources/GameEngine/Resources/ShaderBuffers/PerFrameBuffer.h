#pragma once
#include "Types.h"

namespace GameEngine
{
struct PerFrameBuffer
{
    mat4 ProjectionViewMatrix;
    mat4 ToShadowMapSpace;
    AlignWrapper<vec3> cameraPosition;
};
}  // namespace GameEngine
