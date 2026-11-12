#pragma once
#include "VulkanContext.hpp"

namespace GraphicsApi::Vulkan
{
void CopyBufferToImage(const VulkanContext&, VkBuffer, VkImage, const VkBufferImageCopy&);
VkDescriptorType GetDescriptorType(BufferType);
bool ValidateDrawCallBufferBindings(const VulkanContext&, const VulkanProgram&, const VulkanDrawCall&);
bool AcquireNextSwapChainImage(VulkanContext&, uint32&);
bool PresentSwapChainImage(VulkanContext&, uint32);
void WaitForDeviceIdle(VulkanContext&);
bool CreateRenderPass(VulkanContext&);
bool CreateFramebuffers(VulkanContext&);
bool CreateCommandPoolAndBuffers(VulkanContext&);
bool InitializeRenderingResources(VulkanContext&);
void CleanupSwapChainResources(VulkanContext&);
bool BeginCommandBuffer(VkCommandBuffer);
bool BeginRenderPass(VulkanContext&, VkCommandBuffer, uint32);
uint32 FindMemoryType(VulkanContext&, uint32, VkMemoryPropertyFlags);
bool CreateBuffer(VulkanContext&, VkDeviceSize, VkBufferUsageFlags, VkMemoryPropertyFlags, VkBuffer&, VkDeviceMemory&);
VkCommandBuffer BeginSingleTimeCommands(VulkanContext&);
void EndSingleTimeCommands(VulkanContext&, VkCommandBuffer);
bool CreateImage(VulkanContext&, uint32, uint32, VkFormat, VkImageTiling, VkImageUsageFlags, VkMemoryPropertyFlags, VkImage&,
                 VkDeviceMemory&);
void TransitionImageLayout(VulkanContext&, VkImage, [[maybe_unused]] VkFormat, VkImageLayout, VkImageLayout);
void CopyBufferToImage(VulkanContext&, VkBuffer, VkImage, uint32, uint32);
VkImageView CreateImageView(VkDevice, VkImage, VkFormat);
bool UploadBufferData(VulkanContext&, VkBuffer, VkDeviceMemory, const void*, VkDeviceSize);
VkSampler CreateTextureSampler(VulkanContext&);
std::vector<VkDescriptorSet> PrepareDrawCallDescriptorSets(VulkanContext&, VkDescriptorPool,
                                                           const std::vector<VkDescriptorSetLayout>&, const PipelineConfig&,
                                                           const std::map<uint32, VulkanShaderBuffer>&,
                                                           const std::map<uint32, IdType>&);
void ExecuteDrawCall(VulkanContext&, VkCommandBuffer, VkDescriptorPool, const VulkanProgram&, const VulkanDrawCall&);
void RecordDrawCalls(VulkanContext&, VkCommandBuffer, uint32);
bool EndRenderPass(VkCommandBuffer);
bool EndCommandBuffer(VkCommandBuffer);
bool SubmitCommandBuffer(VulkanContext&, VkCommandBuffer);
ID CreateQuad(VulkanContext&);
}  // namespace GraphicsApi::Vulkan
