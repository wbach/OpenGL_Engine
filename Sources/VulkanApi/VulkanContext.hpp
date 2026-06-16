#pragma once
#include <Types.h>
#include <vulkan/vulkan.h>

#include <functional>
#include <map>
#include <unordered_map>
#include <vector>

#include "IdPool.h"
#include "VulkanMesh.h"
#include "VulkanProgram.h"
#include "VulkanShaderBuffer.h"

namespace GraphicsApi::Vulkan
{
struct VulkanTexture
{
    VkImage image         = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkImageView imageView = VK_NULL_HANDLE;
};
struct RenderState
{
    uint32 activeProgramId = 0u;
    std::map<uint32, uint32> boundShaderBuffers;
};

struct DynamicUniformBuffer
{
    VkBuffer buffer       = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    void* mappedData      = nullptr;
    uint32 totalSize      = 0u;
    uint32 currentOffset  = 0u;
    uint32 dynamicOffset  = 0u;
    uint32 alignment      = 0u;
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

    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;

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
    Utils::IdPool programsPoolId;    // zalezy nam na zachowaniu kolejnosci wstawiania shader bufferow, bo wlasnie w tej kolejnosci beda one wiazane do descriptor setu

    std::unordered_map<IdType, VulkanMesh> meshes;
    Utils::IdPool meshesPoolId;

    std::unordered_map<IdType, VulkanShaderBuffer> shaderBuffers;
    Utils::IdPool shaderBuffersPoolId;

    std::unordered_map<IdType, VulkanTexture> textures;
    Utils::IdPool texturesPoolId;

    RenderState currentRenderState;

    IdType globalDynamicUBOId = 0u;
    GraphicsApi::Vulkan::DynamicUniformBuffer globalDynamicUBO;

    uint32 activeStaticAllocationsCount = 0;
};
}  // namespace GraphicsApi::Vulkan
