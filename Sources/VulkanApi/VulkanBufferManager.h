#pragma once
#include "VulkanContext.hpp"

namespace GraphicsApi::Vulkan
{
class VulkanBufferManager
{
public:
    explicit VulkanBufferManager(VulkanContext&);

    bool CreateBuffer(VkDeviceSize, VkBufferUsageFlags, VkMemoryPropertyFlags, VkBuffer&, VkDeviceMemory&);
    bool UploadBufferData(VkBuffer, VkDeviceMemory, const void*, VkDeviceSize);

private:
    VulkanContext& context;
};
}  // namespace GraphicsApi::Vulkan
