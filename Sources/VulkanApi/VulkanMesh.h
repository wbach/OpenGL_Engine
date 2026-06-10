#pragma once

#include <vulkan/vulkan.h>

#include "GraphicsApi/MeshRawData.h"
#include "GraphicsApi/RenderType.h"
#include "Types.h"

namespace GraphicsApi::Vulkan
{
struct VulkanMesh
{
    GraphicsApi::MeshRawData meshData{};
    GraphicsApi::RenderType renderType = GraphicsApi::RenderType::TRIANGLES;

    uint32 vertexCount = 0;
    bool useIndices = false;

    VkBuffer vertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
    VkBuffer indexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;
};
}  // namespace GraphicsApi::Vulkan
