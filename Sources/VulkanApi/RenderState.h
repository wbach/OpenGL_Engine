#pragma once
#include "VulkanTypes.h"

namespace GraphicsApi::Vulkan
{
struct RenderState
{
    uint32 activeProgramId   = 0u;
    uint32 activeTextureUnit = 0u;
    VulkanShaderBufferBindings boundShaderBuffers;
    VulkanTextureBindings boundTextures;
};
}  // namespace GraphicsApi::Vulkan
