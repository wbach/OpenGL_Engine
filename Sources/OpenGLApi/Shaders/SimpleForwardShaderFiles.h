#pragma once
#include "GraphicsApi/ShaderFiles.h"
#include "GraphicsApi/ShaderProgramType.h"
#include <optional>

namespace OpenGLApi
{
std::optional<GraphicsApi::ShadersFiles> GetSimpleForwardShaderFiles(GraphicsApi::ShaderProgramType shaderType);
} // namespace OpenGLApi
