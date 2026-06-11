#pragma once
#include <Types.h>
#include <vulkan/vulkan.h>

namespace GraphicsApi::Vulkan
{
struct VulkanDrawCall
{
    IdType meshId;
    std::unordered_map<uint32, uint32> boundShaderBuffers;
};

struct VulkanProgram
{
    std::vector<VkDescriptorPool> descriptorPools;
    std::vector<VkDescriptorSet> descriptorSets;
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    VkPipelineLayout layout                   = VK_NULL_HANDLE;
    VkPipeline pipeline                       = VK_NULL_HANDLE;

    std::vector<VulkanDrawCall> drawCalls;
};
}  // namespace GraphicsApi::Vulkan
