#pragma once
#include "GraphicsApi/ShaderBufferVariable.h"
#include "Types.h"

namespace GameEngine
{
struct PerResizeBuffer
{
    mat4 ProjectionMatrix;
};
// clang-format off
const static std::vector<GraphicsApi::ShaderBufferVariable> PER_RESIZE_NAMES
{
    {GraphicsApi::VariableType::MAT4, "PerResize.ProjectionMatrix"}
};
// clang-format on
}  // namespace GameEngine
