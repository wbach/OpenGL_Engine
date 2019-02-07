#pragma once
#include "GraphicsApi/ShaderBufferVariable.h"
#include "Types.h"

namespace GameEngine
{
struct PerAppBuffer
{
    bool UseFakeLighting;
    vec3 ShadowVariables;
    vec4 ClipPlane;
};
// clang-format off
const static std::vector<GraphicsApi::ShaderBufferVariable> PER_APP_NAMES
{
    {GraphicsApi::VariableType::BOOL, "PerApp.UseFakeLighting" },
    {GraphicsApi::VariableType::VEC3, "PerApp.ShadowVariables"},
    {GraphicsApi::VariableType::VEC4, "PerApp.ClipPlane"}
};
// clang-format on
}  // namespace GameEngine
