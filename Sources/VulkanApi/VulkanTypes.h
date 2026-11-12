#pragma once
#include <Types.h>

#include <array>

#include "VulkanShaderBuffer.h"

namespace GraphicsApi::Vulkan
{
constexpr uint32 MAX_DESCRIPTOR_BINDINGS = 30;
using VulkanShaderBufferBindings         = std::array<VulkanShaderBuffer, MAX_DESCRIPTOR_BINDINGS>;
using VulkanTextureBindings              = std::array<IdType, MAX_DESCRIPTOR_BINDINGS>;
}  // namespace GraphicsApi::Vulkan