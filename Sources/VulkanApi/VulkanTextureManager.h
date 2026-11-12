#pragma once
#include "VulkanContext.hpp"

namespace GraphicsApi::Vulkan
{
class VulkanBufferManager;
class VulkanTextureManager
{
public:
    using Textures = std::unordered_map<IdType, VulkanTexture>;

    explicit VulkanTextureManager(VulkanContext&, VulkanBufferManager&);

    GraphicsApi::ID CreateTexture(const Utils::Image&, TextureFilter, TextureMipmap);
    void DeleteTexture(IdType);
    const Textures GetTextures() const;

private:
    bool CreateImage(uint32, uint32, VkFormat, VkImageTiling, VkImageUsageFlags, VkMemoryPropertyFlags, VkImage&,
                     VkDeviceMemory&);
    void CopyBufferToImage(VkBuffer, VkImage, const VkBufferImageCopy&);
    void CopyBufferToImage(VkBuffer, VkImage, uint32, uint32);
    VkImageView CreateImageView(VkDevice, VkImage, VkFormat);
    VkSampler CreateTextureSampler();
    void TransitionImageLayout(VkImage, VkFormat, VkImageLayout, VkImageLayout);

private:
    VulkanContext& context;
    VulkanBufferManager& bufferManager;

    Textures textures;
};
}  // namespace GraphicsApi::Vulkan
