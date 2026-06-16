#pragma once
#include <Types.h>
#include <vulkan/vulkan.h>
#include <map>
#include <variant>

namespace GraphicsApi::Vulkan
{
struct VulkanProceduralDraw
{
    uint32 count{0};
};

struct VulkanMeshDraw
{
    IdType id{0};
};
struct VulkanDrawCall
{
    std::variant<VulkanMeshDraw, VulkanProceduralDraw> drawCommand;
    std::map<uint32, uint32> boundShaderBuffers;
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
