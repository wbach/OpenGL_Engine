#pragma once
#include "VulkanContext.hpp"

namespace GraphicsApi::Vulkan
{
VkDescriptorType GetDescriptorType(BufferType);
bool AcquireNextSwapChainImage(VulkanContext&, uint32&);
bool PresentSwapChainImage(VulkanContext&, uint32);
void WaitForDeviceIdle(VulkanContext&);
bool CreateRenderPass(VulkanContext&);
bool CreateFramebuffers(VulkanContext&);
bool CreateCommandPoolAndBuffers(VulkanContext&);
bool InitializeRenderingResources(VulkanContext&);
void CleanupSwapChainResources(VulkanContext&);
bool BeginCommandBuffer(VkCommandBuffer);
uint32 FindMemoryType(VulkanContext&, uint32, VkMemoryPropertyFlags);
VkCommandBuffer BeginSingleTimeCommands(VulkanContext&);
void EndSingleTimeCommands(VulkanContext&, VkCommandBuffer);
bool EndCommandBuffer(VkCommandBuffer);
}  // namespace GraphicsApi::Vulkan
