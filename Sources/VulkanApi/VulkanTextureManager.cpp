#include "VulkanTextureManager.h"

#include <Logger/Log.h>

#include "VulkanBufferManager.h"
#include "VulkanUtils.h"

namespace GraphicsApi::Vulkan
{
VulkanTextureManager::VulkanTextureManager(VulkanContext& context, VulkanBufferManager& bufferManager)
    : context(context)
    , bufferManager(bufferManager)
    , texturesPoolId(1)
{
}
bool VulkanTextureManager::CreateImage(uint32 width, uint32 height, VkFormat format, VkImageTiling tiling,
                                       VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image,
                                       VkDeviceMemory& imageMemory)
{
    VkImageCreateInfo imageInfo = {.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                                   .imageType     = VK_IMAGE_TYPE_2D,
                                   .format        = format,
                                   .extent        = {.width = width, .height = height, .depth = 1},
                                   .mipLevels     = 1,
                                   .arrayLayers   = 1,
                                   .samples       = VK_SAMPLE_COUNT_1_BIT,
                                   .tiling        = tiling,
                                   .usage         = usage,
                                   .sharingMode   = VK_SHARING_MODE_EXCLUSIVE,
                                   .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED};

    if (vkCreateImage(context.device, &imageInfo, nullptr, &image) != VK_SUCCESS)
    {
        LOG_ERROR << "Failed to create image!";
        return false;
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(context.device, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                                      .allocationSize  = memRequirements.size,
                                      .memoryTypeIndex = FindMemoryType(context, memRequirements.memoryTypeBits, properties)};

    if (vkAllocateMemory(context.device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
    {
        LOG_ERROR << "Failed to allocate image memory!";
        return false;
    }

    vkBindImageMemory(context.device, image, imageMemory, 0);
    return true;
}
void VulkanTextureManager::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32 width, uint32 height)
{
    VkCommandBuffer commandBuffer = BeginSingleTimeCommands(context);

    VkBufferImageCopy region = {
        .bufferOffset      = 0,
        .bufferRowLength   = 0,
        .bufferImageHeight = 0,
        .imageSubresource  = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .mipLevel = 0, .baseArrayLayer = 0, .layerCount = 1},
        .imageOffset       = {0, 0, 0},
        .imageExtent       = {width, height, 1}};

    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    EndSingleTimeCommands(context, commandBuffer);
}
void VulkanTextureManager::CopyBufferToImage(VkBuffer buffer, VkImage image, const VkBufferImageCopy& region)
{
    VkCommandBufferAllocateInfo allocInfo{.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                                          .commandPool        = context.commandPool,
                                          .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                          .commandBufferCount = 1};

    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    vkAllocateCommandBuffers(context.device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                                       .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT};
    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO, .commandBufferCount = 1, .pCommandBuffers = &commandBuffer};
    vkQueueSubmit(context.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(context.graphicsQueue);
    vkFreeCommandBuffers(context.device, context.commandPool, 1, &commandBuffer);
}
VkImageView VulkanTextureManager::CreateImageView(VkDevice device, VkImage image, VkFormat format)
{
    VkImageViewCreateInfo viewInfo = {
        .sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image            = image,
        .viewType         = VK_IMAGE_VIEW_TYPE_2D,
        .format           = format,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1}};

    VkImageView imageView;
    if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
    {
        LOG_ERROR << "Failed to create texture image view!";
        return VK_NULL_HANDLE;
    }

    return imageView;
}
VkSampler VulkanTextureManager::CreateTextureSampler()
{
    VkSamplerCreateInfo samplerInfo{.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                                    .magFilter               = VK_FILTER_LINEAR,
                                    .minFilter               = VK_FILTER_LINEAR,
                                    .mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR,
                                    .addressModeU            = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                                    .addressModeV            = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                                    .addressModeW            = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                                    .mipLodBias              = 0.0f,
                                    .anisotropyEnable        = VK_FALSE,
                                    .maxAnisotropy           = 1.0f,
                                    .compareEnable           = VK_FALSE,
                                    .compareOp               = VK_COMPARE_OP_ALWAYS,
                                    .minLod                  = 0.0f,
                                    .maxLod                  = 0.0f,
                                    .borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
                                    .unnormalizedCoordinates = VK_FALSE};

    VkSampler sampler = VK_NULL_HANDLE;
    if (vkCreateSampler(context.device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
    {
        LOG_ERROR << "Error: Failed to create VkSampler for texture!\n";
        return VK_NULL_HANDLE;
    }
    return sampler;
}
void VulkanTextureManager::TransitionImageLayout(VkImage image, [[maybe_unused]] VkFormat format, VkImageLayout oldLayout,
                                                 VkImageLayout newLayout)
{
    VkCommandBuffer commandBuffer = BeginSingleTimeCommands(context);

    VkImageMemoryBarrier barrier = {
        .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .srcAccessMask       = 0,
        .dstAccessMask       = 0,
        .oldLayout           = oldLayout,
        .newLayout           = newLayout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image               = image,
        .subresourceRange    = {
               .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1}};

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED and newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        sourceStage           = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL and newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        sourceStage           = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage      = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else
    {
        LOG_ERROR << "Unsupported layout transition!";
        EndSingleTimeCommands(context, commandBuffer);
        return;
    }

    vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    EndSingleTimeCommands(context, commandBuffer);
}
GraphicsApi::ID VulkanTextureManager::CreateTexture(const Utils::Image& image, [[maybe_unused]] TextureFilter filter,
                                                    [[maybe_unused]] TextureMipmap mipmap)
{
    constexpr VkDeviceSize bytesPerPixel = 4;
    const VkDeviceSize rowSize           = static_cast<VkDeviceSize>(image.width) * bytesPerPixel;
    const VkDeviceSize pitch             = image.pitch.value_or(static_cast<size_t>(rowSize));

    if (image.width == 0 || image.height == 0 || pitch < rowSize || pitch % bytesPerPixel != 0)
    {
        return {};
    }

    const VkDeviceSize imageSize = pitch * image.height;

    VkBuffer stagingBuffer             = VK_NULL_HANDLE;
    VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;

    bufferManager.CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
                               stagingBufferMemory);

    void* data = nullptr;

    if (vkMapMemory(context.device, stagingBufferMemory, 0, imageSize, 0, &data) != VK_SUCCESS)
    {
        vkDestroyBuffer(context.device, stagingBuffer, nullptr);
        vkFreeMemory(context.device, stagingBufferMemory, nullptr);
        return {};
    }

    std::memcpy(data, image.getRawDataPtr(), static_cast<size_t>(imageSize));

    vkUnmapMemory(context.device, stagingBufferMemory);

    VulkanTexture texture;

    constexpr VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;

    if (not CreateImage(image.width, image.height, format, VK_IMAGE_TILING_OPTIMAL,
                        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        texture.image, texture.memory))
    {
        vkDestroyBuffer(context.device, stagingBuffer, nullptr);
        vkFreeMemory(context.device, stagingBufferMemory, nullptr);
        return {};
    }

    TransitionImageLayout(texture.image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    VkBufferImageCopy region{
        .bufferOffset      = 0,
        .bufferRowLength   = static_cast<uint32>(pitch / bytesPerPixel),
        .bufferImageHeight = 0,
        .imageSubresource =
            VkImageSubresourceLayers{
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .mipLevel = 0, .baseArrayLayer = 0, .layerCount = 1},
        .imageOffset = {0, 0, 0},
        .imageExtent = {image.width, image.height, 1}};

    CopyBufferToImage(stagingBuffer, texture.image, region);

    TransitionImageLayout(texture.image, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(context.device, stagingBuffer, nullptr);
    vkFreeMemory(context.device, stagingBufferMemory, nullptr);

    texture.imageView = CreateImageView(context.device, texture.image, format);

    if (texture.imageView == VK_NULL_HANDLE)
    {
        vkDestroyImage(context.device, texture.image, nullptr);
        vkFreeMemory(context.device, texture.memory, nullptr);
        return {};
    }

    texture.sampler = CreateTextureSampler();

    if (texture.sampler == VK_NULL_HANDLE)
    {
        vkDestroyImageView(context.device, texture.imageView, nullptr);

        vkDestroyImage(context.device, texture.image, nullptr);
        vkFreeMemory(context.device, texture.memory, nullptr);
        return {};
    }

    const auto textureId = texturesPoolId.getId();
    textures[textureId]  = std::move(texture);

    return textureId;
}
void VulkanTextureManager::DeleteTexture(IdType id)
{
    auto iter = textures.find(id);
    if (iter != textures.end())
    {
        // TO DO: vk remove

        textures.erase(iter);
        texturesPoolId.releaseId(id);
    }
}
const VulkanTextureManager::Textures VulkanTextureManager::GetTextures() const
{
    return textures;
}
}  // namespace GraphicsApi::Vulkan
