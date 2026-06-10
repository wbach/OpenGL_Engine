#pragma once
#include <Types.h>
#include <vulkan/vulkan.h>

namespace GraphicsApi::Vulkan
{
struct VulkanShaderBuffer
{
    VkBuffer buffer       = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    void* mappedData      = nullptr;
    uint32 size           = 0u;
    uint32 bindLocation   = 0u;
};
}  // namespace GraphicsApi::Vulkan
