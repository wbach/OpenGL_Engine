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
    VkSampler sampler     = VK_NULL_HANDLE;
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

enum class ObjectType
{
    TEXTURE_2D,
    TEXTURE_2D_ARRAY,
    TEXTURE_CUBE_MAP,
    FRAME_BUFFER,
    RENDER_BUFFER,
    SHADER_PROGRAM,
    SHADER_BUFFER,
    MESH
};

class VulkanIdPool
{
public:
    VulkanIdPool()
        : objectsIdPool(1)
    {
    }
    IdType AccquireNewId(ObjectType type)
    {
        auto newID = objectsIdPool.getId();
        createdObjectIds.insert({newID, type});
        return newID;
    }

    std::optional<ObjectType> GetObjectType(IdType id)
    {
        auto iter = createdObjectIds.find(id);
        if (iter != createdObjectIds.end())
        {
            return iter->second;
        }

        return std::nullopt;
    }

    void ReleaseId(IdType id)
    {
        objectsIdPool.releaseId(id);
        createdObjectIds.erase(id);
    }

private:
    Utils::IdPool objectsIdPool;
    std::unordered_map<IdType, ObjectType> createdObjectIds;
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

    std::unordered_map<IdType, VulkanMesh> meshes;
    std::unordered_map<IdType, VulkanShaderBuffer> shaderBuffers;
    IdType globalDynamicUBOId = 0u;
    GraphicsApi::Vulkan::DynamicUniformBuffer globalDynamicUBO;

    uint32 activeStaticAllocationsCount = 0;

    VulkanIdPool idPool;
};
}  // namespace GraphicsApi::Vulkan
