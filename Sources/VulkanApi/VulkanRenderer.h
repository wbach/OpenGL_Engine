#pragma once
#include "RenderState.h"
#include "VulkanContext.hpp"
namespace GraphicsApi::Vulkan
{
class VulkanTextureManager;
class VulkanRenderer
{
public:
    using ImageIndex = uint32;

    explicit VulkanRenderer(VulkanContext&, VulkanTextureManager&);

public:
    void PrepareFrame();
    void RenderFrame(ImageIndex);
    void DrawCall(VulkanDrawCommand drawCommand);

private:
    bool BeginRenderPass(VkCommandBuffer, uint32);
    std::vector<VkDescriptorSet> PrepareDrawCallDescriptorSets(VkDescriptorPool, const std::vector<VkDescriptorSetLayout>&,
                                                               const PipelineConfig&, const VulkanShaderBufferBindings&,
                                                               const VulkanTextureBindings&);
    void ExecuteDrawCall(VkCommandBuffer, VkDescriptorPool, const VulkanProgram&, const VulkanDrawCall&);
    void RecordDrawCalls(VkCommandBuffer, uint32);
    bool EndRenderPass(VkCommandBuffer);
    bool SubmitCommandBuffer(VkCommandBuffer);
    void FilterDrawCallForProgram(const VulkanProgram&, VulkanDrawCall&) const;

public:
    RenderState currentRenderState;

private:
    VulkanContext& context;
    VulkanTextureManager& textureManager;

    std::vector<VulkanDrawCall> pendingDrawCalls;
};
}  // namespace GraphicsApi::Vulkan
