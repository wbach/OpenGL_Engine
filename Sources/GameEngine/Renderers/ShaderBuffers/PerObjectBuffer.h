#pragma once
#include "GraphicsApi/ShaderBufferVariable.h"
#include "Types.h"

namespace GameEngine
{
const int MAX_BONES = 100;

struct PerObjectBuffer
{
    bool UseBoneTransform;
    bool UseNormalMap;
    uint32 NumberOfRows;
    vec2 TextureOffset;
    mat4 TransformationMatrix;
    mat4 BonesTransforms[MAX_BONES];
};
// clang-format off
const static std::vector<GraphicsApi::ShaderBufferVariable> PER_OBJECT_NAMES
{
    {GraphicsApi::VariableType::BOOL, "PerObject.UseBoneTransform"},
    {GraphicsApi::VariableType::BOOL, "PerObject.UseNormalMap"},
    {GraphicsApi::VariableType::INT, "PerObject.NumberOfRows"},
    {GraphicsApi::VariableType::VEC2, "PerObject.TextureOffset"},
    {GraphicsApi::VariableType::MAT4, "PerObject.TransformationMatrix"},
    {GraphicsApi::VariableType::MAT4_100, "PerObject.BonesTransforms"}
};
// clang-format on
}  // namespace GameEngine
