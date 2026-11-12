#include "VulkanBufferManager.h"

#include <Logger/Log.h>

#include "VulkanUtils.h"

namespace GraphicsApi::Vulkan
{
VulkanBufferManager::VulkanBufferManager(VulkanContext& context)
    : context(context)
{
}
bool VulkanBufferManager::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                                       VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
    VkBufferCreateInfo bufferInfo{
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO, .size = size, .usage = usage, .sharingMode = VK_SHARING_MODE_EXCLUSIVE};

    if (vkCreateBuffer(context.device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
    {
        LOG_ERROR << "Error: Failed to create buffer.\n";
        return false;
    }

    VkMemoryRequirements memRequirements{};
    vkGetBufferMemoryRequirements(context.device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                                   .allocationSize  = memRequirements.size,
                                   .memoryTypeIndex = FindMemoryType(context, memRequirements.memoryTypeBits, properties)};

    if (vkAllocateMemory(context.device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
    {
        LOG_ERROR << "Error: Failed to allocate buffer memory.\n";
        vkDestroyBuffer(context.device, buffer, nullptr);
        return false;
    }

    vkBindBufferMemory(context.device, buffer, bufferMemory, 0);
    return true;
}
bool VulkanBufferManager::UploadBufferData(VkBuffer buffer, VkDeviceMemory bufferMemory, const void* data, VkDeviceSize size)
{
    void* mapped = nullptr;
    if (vkMapMemory(context.device, bufferMemory, 0, size, 0, &mapped) != VK_SUCCESS)
    {
        return false;
    }

    std::memcpy(mapped, data, static_cast<size_t>(size));
    vkUnmapMemory(context.device, bufferMemory);
    return true;
}
}  // namespace GraphicsApi::Vulkan
