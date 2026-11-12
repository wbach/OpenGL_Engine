#pragma once
#include <Types.h>
#include <vulkan/vulkan.h>

#include <map>
#include <variant>

#include "BufferType.h"
#include "PipelineConfig.h"
#include "VulkanTypes.h"


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

using VulkanDrawCommand = std::variant<VulkanMeshDraw, VulkanProceduralDraw>;

struct VulkanDrawCall
{
    uint32 programId{0};
    VulkanDrawCommand drawCommand;
    VulkanShaderBufferBindings boundShaderBuffers;
    VulkanTextureBindings boundTextures;
};

struct VulkanProgram
{
    std::vector<VkDescriptorPool> descriptorPools;
    std::vector<VkDescriptorSet> descriptorSets;
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
    VkPipelineLayout layout = VK_NULL_HANDLE;
    VkPipeline pipeline     = VK_NULL_HANDLE;
    PipelineConfig pipelineConfig;
};
}  // namespace GraphicsApi::Vulkan
