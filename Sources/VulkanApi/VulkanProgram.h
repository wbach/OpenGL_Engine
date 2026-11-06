#pragma once
#include <Types.h>
#include <vulkan/vulkan.h>

#include <map>
#include <variant>

#include "BufferType.h"
#include "VulkanShaderBuffer.h"
#include "PipelineConfig.h"

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

using VulkanShaderBufferBindings = std::map<uint32, VulkanShaderBuffer>;

struct VulkanDrawCall
{
    uint32 programId{0};
    std::variant<VulkanMeshDraw, VulkanProceduralDraw> drawCommand;
    VulkanShaderBufferBindings boundShaderBuffers;
};

struct VulkanProgram
{
    std::vector<VkDescriptorPool> descriptorPools;
    std::vector<VkDescriptorSet> descriptorSets;
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    VkPipelineLayout layout                   = VK_NULL_HANDLE;
    VkPipeline pipeline                       = VK_NULL_HANDLE;
    PipelineConfig pipelineConfig;
};
}  // namespace GraphicsApi::Vulkan
