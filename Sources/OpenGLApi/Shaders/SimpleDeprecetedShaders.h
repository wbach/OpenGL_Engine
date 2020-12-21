#pragma once
#include "GraphicsApi/ShaderFiles.h"
#include "GraphicsApi/ShaderProgramType.h"
#include <optional>

namespace OpenGLApi
{
std::optional<GraphicsApi::ShadersFiles> GetSimpleDeprecetedShaders(GraphicsApi::ShaderProgramType shaderType);
} // namespace OpenGLApi
