#pragma once
#include "GraphicsApi/ShaderFiles.h"
#include "GraphicsApi/ShaderProgramType.h"

namespace DirectX
{
GraphicsApi::ShadersFiles GetDxForwardShaderFiles(GraphicsApi::ShaderProgramType shaderType);
bool FileNameExist(GraphicsApi::ShaderProgramType shaderType);
}  // namespace DirectX
