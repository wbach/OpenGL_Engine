#pragma once
#include <optional>

#include "GraphicsApi/ShaderProgramType.h"
#include "PipelineConfig.h"

namespace GraphicsApi::Vulkan
{
std::optional<PipelineConfig> GetSimpleForwardShaderFiles(GraphicsApi::ShaderProgramType shaderType);
}  // namespace GraphicsApi::Vulkan
