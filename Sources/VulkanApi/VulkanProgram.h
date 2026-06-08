#pragma once
#include <Types.h>
#include <vulkan/vulkan.h>

namespace GraphicsApi::Vulkan
{
struct VulkanDrawCall
{
    IdType meshId;
    // Tutaj w przyszłości dojdą np. macierze transformacji (pozycja, rotacja)
};

struct VulkanProgram
{
    VkPipelineLayout layout = VK_NULL_HANDLE;
    VkPipeline pipeline     = VK_NULL_HANDLE;

    std::vector<VulkanDrawCall> drawCalls;
};
}  // namespace GraphicsApi::Vulkan
