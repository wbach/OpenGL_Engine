#pragma once
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
    void RenderFrame(ImageIndex);

private:
    bool BeginRenderPass(VkCommandBuffer, uint32);
    std::vector<VkDescriptorSet> PrepareDrawCallDescriptorSets(VkDescriptorPool, const std::vector<VkDescriptorSetLayout>&,
                                                               const PipelineConfig&, const std::map<uint32, VulkanShaderBuffer>&,
                                                               const std::map<uint32, IdType>&);
    void ExecuteDrawCall(VkCommandBuffer, VkDescriptorPool, const VulkanProgram&, const VulkanDrawCall&);
    void RecordDrawCalls(VkCommandBuffer, uint32);
    bool EndRenderPass(VkCommandBuffer);
    bool ValidateDrawCallBufferBindings(const VulkanProgram&, const VulkanDrawCall&);
    bool SubmitCommandBuffer(VkCommandBuffer);

private:
    VulkanContext& context;
    VulkanTextureManager& textureManager;
};
}  // namespace GraphicsApi::Vulkan
