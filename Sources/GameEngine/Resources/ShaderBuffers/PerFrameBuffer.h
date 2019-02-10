#pragma once
#include "Types.h"

namespace GameEngine
{
struct PerFrameBuffer
{
    mat4 ViewMatrix;
    mat4 ToShadowMapSpace;
};
}  // namespace GameEngine
