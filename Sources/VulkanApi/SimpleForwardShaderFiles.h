#pragma once
#include <optional>

#include "GraphicsApi/ShaderFiles.h"
#include "GraphicsApi/ShaderProgramType.h"

namespace GraphicsApi::Vulkan
{
std::optional<GraphicsApi::ShadersFiles> GetSimpleForwardShaderFiles(GraphicsApi::ShaderProgramType shaderType);
}  // namespace GraphicsApi::Vulkan
