#pragma once
#include <Types.h>
#include <vulkan/vulkan.h>

#include <functional>
#include <unordered_map>
#include <vector>

#include "IdPool.h"
#include "VulkanMesh.h"
#include "VulkanProgram.h"
#include "VulkanShaderBuffer.h"
namespace GraphicsApi::Vulkan
{
struct RenderState
{
    uint32 activeProgramId = 0u;
    std::unordered_map<uint32, uint32> boundShaderBuffers;
};

struct VulkanContext
{
    void ClearResources();

    VkInstance instance             = VK_NULL_HANDLE;
    VkSurfaceKHR surface            = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    VkDevice device       = VK_NULL_HANDLE;
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    VkQueue presentQueue  = VK_NULL_HANDLE;

    uint32 graphicsFamilyIndex = 0xFFFFFFFF;
    uint32 presentFamilyIndex  = 0xFFFFFFFF;

    VkSwapchainKHR swapChain = VK_NULL_HANDLE;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;

    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;

    VkSemaphore imageAvailableSemaphore = VK_NULL_HANDLE;
    VkSemaphore renderFinishedSemaphore = VK_NULL_HANDLE;

    VkRenderPass renderPass = VK_NULL_HANDLE;
    std::vector<VkFramebuffer> framebuffers;

    VkCommandPool commandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> commandBuffers;

    Color backgroundColor{0.9f, 0.0f, 0.0f, 1.0f};

    std::unordered_map<IdType, VulkanProgram> programs;
    Utils::IdPool programsPoolId;

    std::unordered_map<IdType, VulkanMesh> meshes;
    Utils::IdPool meshesPoolId;

    std::unordered_map<IdType, VulkanShaderBuffer> shaderBuffers;
    Utils::IdPool shaderBuffersPoolId;

    RenderState currentRenderState;
};
}  // namespace GraphicsApi::Vulkan
