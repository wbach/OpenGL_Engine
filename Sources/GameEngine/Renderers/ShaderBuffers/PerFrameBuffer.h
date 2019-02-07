#pragma once
#include "GraphicsApi/ShaderBufferVariable.h"
#include "Types.h"

namespace GameEngine
{
struct PerFrameBuffer
{
    mat4 ViewMatrix;
    mat4 ToShadowMapSpace;
};
// clang-format off
const static std::vector<GraphicsApi::ShaderBufferVariable> PER_FRAME_NAMES
{
    {GraphicsApi::VariableType::MAT4, "PerFrame.ViewMatrix" },
    {GraphicsApi::VariableType::MAT4, "PerFrame.ToShadowMapSpace" },
};
// clang-format on
}  // namespace GameEngine
