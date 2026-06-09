#pragma once
#include "GraphicsApi/ShaderFiles.h"
#include "GraphicsApi/ShaderProgramType.h"

namespace GraphicsApi::Dx11
{
GraphicsApi::ShadersFiles GetDxForwardShaderFiles(GraphicsApi::ShaderProgramType shaderType);
bool FileNameExist(GraphicsApi::ShaderProgramType shaderType);
}  // namespace GraphicsApi::Dx11
