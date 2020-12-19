#pragma once
#include "GraphicsApi/ShaderFiles.h"
#include "GraphicsApi/ShaderProgramType.h"
#include <optional>

namespace OpenGLApi
{
std::optional<GraphicsApi::ShadersFiles> GetFullDefferedShaderFiles(GraphicsApi::ShaderProgramType shaderType);
} // namespace OpenGLApi
