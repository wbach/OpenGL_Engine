#pragma once
#include "GraphicsApi/ShaderFiles.h"
#include "GraphicsApi/ShadersTypes.h"

namespace DirectX
{
GraphicsApi::ShadersFiles GetDxForwardShaderFiles(GraphicsApi::Shaders shaderType);
bool FileNameExist(GraphicsApi::Shaders shaderType);
}  // namespace DirectX
