#pragma once
#include "GraphicsApi/ShaderFiles.h"
#include "GraphicsApi/ShaderProgramType.h"

namespace OpenGLApi
{
std::optional<GraphicsApi::ShadersFiles> GetSimpleDeprecetedShaders(GraphicsApi::ShaderProgramType shaderType);
} // namespace OpenGLApi
