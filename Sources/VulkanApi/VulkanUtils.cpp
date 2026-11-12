#include "VulkanUtils.h"

#include <Logger/Log.h>
#include <Utils/Variant.h>

namespace GraphicsApi::Vulkan
{
void CopyBufferToImage(const VulkanContext& context, VkBuffer buffer, VkImage image, const VkBufferImageCopy& region)
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
VkDescriptorType GetDescriptorType(BufferType type)
{
    switch (type)
    {
        case BufferType::StaticPerObject:
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

        case BufferType::DynamicPerFrame:
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    }

    return VK_DESCRIPTOR_TYPE_MAX_ENUM;
}
bool ValidateDrawCallBufferBindings(const VulkanContext& vkContext, const VulkanProgram& program, const VulkanDrawCall& drawCall)
{
    bool valid = true;

    for (const auto& [binding, shaderBuffer] : drawCall.boundShaderBuffers)
    {
        const auto& descriptorBindings = program.pipelineConfig.descriptorBindings;
        // Find binding in current program configuration.
        const auto configIt =
            std::find_if(descriptorBindings.begin(), descriptorBindings.end(),
                         [binding](const DescriptorBindingConfig& config) { return config.binding == binding; });

        if (configIt == descriptorBindings.end())
        {
            LOG_ERROR << "ShaderBuffer binding mismatch!"
                      << " Program: " << drawCall.programId << ", Binding: " << binding << " is not declared in PipelineConfig.";

            valid = false;
            continue;
        }

        // Currently we support one buffer per binding.
        if (configIt->descriptorCount != 1)
        {
            LOG_ERROR << "Unsupported descriptorCount!"
                      << " Program: " << drawCall.programId << ", Binding: " << binding
                      << ", Count: " << configIt->descriptorCount;

            valid = false;
            continue;
        }

        // Check descriptor type.
        const VkDescriptorType actualType = GetDescriptorType(shaderBuffer.type);

        if (configIt->descriptorType != actualType)
        {
            LOG_ERROR << "ShaderBuffer descriptor type mismatch!"
                      << " Program: " << drawCall.programId << ", Binding: " << binding
                      << ", Expected: " << configIt->descriptorType << ", Actual: " << actualType
                      << ", BufferType: " << static_cast<int>(shaderBuffer.type);

            valid = false;
        }

        // Check logical binding consistency.
        if (shaderBuffer.bindLocation != binding)
        {
            LOG_ERROR << "ShaderBuffer bindLocation mismatch!"
                      << " Program: " << drawCall.programId << ", Map binding: " << binding
                      << ", Buffer bindLocation: " << shaderBuffer.bindLocation;

            valid = false;
        }

        // Check Vulkan handle.
        if (shaderBuffer.buffer == VK_NULL_HANDLE)
        {
            LOG_ERROR << "ShaderBuffer contains NULL VkBuffer!"
                      << " Program: " << drawCall.programId << ", Binding: " << binding;

            valid = false;
        }

        // Check dynamic buffer allocation.
        if (shaderBuffer.type == BufferType::DynamicPerFrame)
        {
            const auto& dUbo = vkContext.globalDynamicUBO;

            const VkDeviceSize offset = shaderBuffer.dynamicOffset;
            const VkDeviceSize size   = shaderBuffer.size;

            if (dUbo.alignment != 0 && offset % dUbo.alignment != 0)
            {
                LOG_ERROR << "Dynamic UBO alignment error!"
                          << " Program: " << drawCall.programId << ", Binding: " << binding << ", Offset: " << offset
                          << ", Alignment: " << dUbo.alignment;

                valid = false;
            }

            if (offset > dUbo.totalSize || size > dUbo.totalSize - offset)
            {
                LOG_ERROR << "Dynamic UBO range out of bounds!"
                          << " Program: " << drawCall.programId << ", Binding: " << binding << ", Offset: " << offset
                          << ", Size: " << size << ", Total: " << dUbo.totalSize;

                valid = false;
            }
        }
    }

    return valid;
}
bool AcquireNextSwapChainImage(VulkanContext& vkContext, uint32& imageIndex)
{
    VkResult result = vkAcquireNextImageKHR(vkContext.device, vkContext.swapChain, UINT64_MAX, vkContext.imageAvailableSemaphore,
                                            VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        return false;
    }

    if (result != VK_SUCCESS and result != VK_SUBOPTIMAL_KHR)
    {
        LOG_ERROR << "Error while acquiring an image from the swapchain! Error code: " << result;
        return false;
    }

    return true;
}
bool PresentSwapChainImage(VulkanContext& vkContext, uint32 imageIndex)
{
    VkPresentInfoKHR presentInfo{.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                                 .waitSemaphoreCount = 1,
                                 .pWaitSemaphores    = &vkContext.renderFinishedSemaphore};

    VkSwapchainKHR swapChains[] = {vkContext.swapChain};
    presentInfo.swapchainCount  = 1;
    presentInfo.pSwapchains     = swapChains;
    presentInfo.pImageIndices   = &imageIndex;

    VkResult result = vkQueuePresentKHR(vkContext.presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR or result == VK_SUBOPTIMAL_KHR)
    {
        return false;
    }

    if (result != VK_SUCCESS)
    {
        LOG_ERROR << "Error: Image presentation failed!\n";
        return false;
    }

    return true;
}
void WaitForDeviceIdle(VulkanContext& vkContext)
{
    vkDeviceWaitIdle(vkContext.device);
}
bool CreateRenderPass(VulkanContext& vkContext)
{
    VkAttachmentDescription colorAttachment{.format         = vkContext.swapChainImageFormat,
                                            .samples        = VK_SAMPLE_COUNT_1_BIT,
                                            .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
                                            .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
                                            .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                                            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                            .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
                                            .finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR};

    VkAttachmentReference colorAttachmentRef{.attachment = 0, .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};

    VkAttachmentDescription depthAttachment{.format         = VK_FORMAT_D32_SFLOAT,
                                            .samples        = VK_SAMPLE_COUNT_1_BIT,
                                            .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
                                            .storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                            .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
                                            .finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};

    VkAttachmentReference depthAttachmentRef{.attachment = 1, .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};

    VkSubpassDescription subpass{.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS,
                                 .colorAttachmentCount    = 1,
                                 .pColorAttachments       = &colorAttachmentRef,
                                 .pDepthStencilAttachment = &depthAttachmentRef};

    VkAttachmentDescription attachments[] = {colorAttachment, depthAttachment};

    VkRenderPassCreateInfo renderPassInfo{
        .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 2,
        .pAttachments    = attachments,
        .subpassCount    = 1,
        .pSubpasses      = &subpass,
    };

    if (vkCreateRenderPass(vkContext.device, &renderPassInfo, nullptr, &vkContext.renderPass) != VK_SUCCESS)
    {
        LOG_ERROR << "Error: Failed to create Render Pass.\n";
        return false;
    }

    return true;
}
bool CreateFramebuffers(VulkanContext& vkContext)
{
    vkContext.framebuffers.resize(vkContext.swapChainImageViews.size());

    for (size_t i = 0; i < vkContext.swapChainImageViews.size(); ++i)
    {
        VkImageView attachments[] = {vkContext.swapChainImageViews[i], vkContext.depthImageView};

        VkFramebufferCreateInfo framebufferInfo{.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                                                .renderPass      = vkContext.renderPass,
                                                .attachmentCount = 2,
                                                .pAttachments    = attachments,
                                                .width           = vkContext.swapChainExtent.width,
                                                .height          = vkContext.swapChainExtent.height,
                                                .layers          = 1};

        if (vkCreateFramebuffer(vkContext.device, &framebufferInfo, nullptr, &vkContext.framebuffers[i]) != VK_SUCCESS)
        {
            LOG_ERROR << "Error: Failed to create Framebuffer for index " << i << "\n";
            return false;
        }
    }

    return true;
}
bool CreateCommandPoolAndBuffers(VulkanContext& vkContext)
{
    VkCommandPoolCreateInfo poolInfo{.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                                     .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                                     .queueFamilyIndex = vkContext.graphicsFamilyIndex};

    if (vkCreateCommandPool(vkContext.device, &poolInfo, nullptr, &vkContext.commandPool) != VK_SUCCESS)
    {
        LOG_ERROR << "Error: Failed to create Command Pool.\n";
        return false;
    }

    vkContext.commandBuffers.resize(vkContext.framebuffers.size());

    VkCommandBufferAllocateInfo allocInfo{.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                                          .commandPool        = vkContext.commandPool,
                                          .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                          .commandBufferCount = static_cast<uint32>(vkContext.commandBuffers.size())};

    if (vkAllocateCommandBuffers(vkContext.device, &allocInfo, vkContext.commandBuffers.data()) != VK_SUCCESS)
    {
        LOG_ERROR << "Error: Failed to allocate Command Buffers.\n";
        return false;
    }

    return true;
}
bool InitializeRenderingResources(VulkanContext& vkContext)
{
    if (!CreateRenderPass(vkContext))
    {
        return false;
    }

    if (!CreateFramebuffers(vkContext))
    {
        return false;
    }

    if (!CreateCommandPoolAndBuffers(vkContext))
    {
        return false;
    }

    return true;
}
void CleanupSwapChainResources(VulkanContext& vkContext)
{
    for (auto framebuffer : vkContext.framebuffers)
    {
        vkDestroyFramebuffer(vkContext.device, framebuffer, nullptr);
    }
    vkContext.framebuffers.clear();

    if (vkContext.commandPool != VK_NULL_HANDLE)
    {
        vkDestroyCommandPool(vkContext.device, vkContext.commandPool, nullptr);
        vkContext.commandPool = VK_NULL_HANDLE;
    }

    if (vkContext.renderPass != VK_NULL_HANDLE)
    {
        vkDestroyRenderPass(vkContext.device, vkContext.renderPass, nullptr);
        vkContext.renderPass = VK_NULL_HANDLE;
    }

    LOG_DEBUG << "Success: Render Pass, Framebuffers and Command Buffers are ready!";
}
bool BeginCommandBuffer(VkCommandBuffer commandBuffer)
{
    VkCommandBufferBeginInfo beginInfo{.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        LOG_ERROR << "Error: Failed to begin recording the command buffer.\n";
        return false;
    }

    return true;
}
bool BeginRenderPass(VulkanContext& vkContext, VkCommandBuffer commandBuffer, uint32 imageIndex)
{
    VkClearValue clearColor[2] = {
        {.color = VkClearColorValue{.float32 = {vkContext.backgroundColor[0], vkContext.backgroundColor[1],
                                                vkContext.backgroundColor[2], vkContext.backgroundColor[3]}}},
        {.depthStencil = VkClearDepthStencilValue{.depth = 1.0f, .stencil = 0}}};

    VkRenderPassBeginInfo renderPassInfo{.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                                         .renderPass      = vkContext.renderPass,
                                         .framebuffer     = vkContext.framebuffers[imageIndex],
                                         .renderArea      = VkRect2D{.offset = {0, 0}, .extent = vkContext.swapChainExtent},
                                         .clearValueCount = 2,
                                         .pClearValues    = clearColor};

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    return true;
}
uint32 FindMemoryType(VulkanContext& vkContext, uint32 typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties{};
    vkGetPhysicalDeviceMemoryProperties(vkContext.physicalDevice, &memProperties);

    for (uint32 i = 0; i < memProperties.memoryTypeCount; ++i)
    {
        if ((typeFilter & (1u << i)) and (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    return 0;
}
bool CreateBuffer(VulkanContext& vkContext, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                  VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
    VkBufferCreateInfo bufferInfo{
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO, .size = size, .usage = usage, .sharingMode = VK_SHARING_MODE_EXCLUSIVE};

    if (vkCreateBuffer(vkContext.device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
    {
        LOG_ERROR << "Error: Failed to create buffer.\n";
        return false;
    }

    VkMemoryRequirements memRequirements{};
    vkGetBufferMemoryRequirements(vkContext.device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize  = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(vkContext, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(vkContext.device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
    {
        LOG_ERROR << "Error: Failed to allocate buffer memory.\n";
        vkDestroyBuffer(vkContext.device, buffer, nullptr);
        return false;
    }

    vkBindBufferMemory(vkContext.device, buffer, bufferMemory, 0);
    return true;
}
VkCommandBuffer BeginSingleTimeCommands(VulkanContext& vkContext)
{
    VkCommandBufferAllocateInfo allocInfo = {.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                                             .commandPool        = vkContext.commandPool,
                                             .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                             .commandBufferCount = 1};

    VkCommandBuffer commandBuffer;
    if (vkAllocateCommandBuffers(vkContext.device, &allocInfo, &commandBuffer) != VK_SUCCESS)
    {
        LOG_ERROR << "Error: Failed to allocate command buffer.\n";
        return VK_NULL_HANDLE;
    }

    VkCommandBufferBeginInfo beginInfo = {.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                                          .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT};

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        LOG_ERROR << "Error: Failed to begin command buffer.\n";
        return VK_NULL_HANDLE;
    }

    return commandBuffer;
}
void EndSingleTimeCommands(VulkanContext& vkContext, VkCommandBuffer commandBuffer)
{
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        LOG_ERROR << "Error: Failed to end command buffer.\n";
        return;
    }

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO, .commandBufferCount = 1, .pCommandBuffers = &commandBuffer};

    vkQueueSubmit(vkContext.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(vkContext.graphicsQueue);
    vkFreeCommandBuffers(vkContext.device, vkContext.commandPool, 1, &commandBuffer);
}
bool CreateImage(VulkanContext& vkContext, uint32 width, uint32 height, VkFormat format, VkImageTiling tiling,
                 VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
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

    if (vkCreateImage(vkContext.device, &imageInfo, nullptr, &image) != VK_SUCCESS)
    {
        LOG_ERROR << "Failed to create image!";
        return false;
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(vkContext.device, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                                      .allocationSize  = memRequirements.size,
                                      .memoryTypeIndex = FindMemoryType(vkContext, memRequirements.memoryTypeBits, properties)};

    if (vkAllocateMemory(vkContext.device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
    {
        LOG_ERROR << "Failed to allocate image memory!";
        return false;
    }

    vkBindImageMemory(vkContext.device, image, imageMemory, 0);
    return true;
}
void TransitionImageLayout(VulkanContext& vkContext, VkImage image, [[maybe_unused]] VkFormat format, VkImageLayout oldLayout,
                           VkImageLayout newLayout)
{
    VkCommandBuffer commandBuffer = BeginSingleTimeCommands(vkContext);

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
        EndSingleTimeCommands(vkContext, commandBuffer);
        return;
    }

    vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    EndSingleTimeCommands(vkContext, commandBuffer);
}
void CopyBufferToImage(VulkanContext& vkContext, VkBuffer buffer, VkImage image, uint32 width, uint32 height)
{
    VkCommandBuffer commandBuffer = BeginSingleTimeCommands(vkContext);

    VkBufferImageCopy region = {
        .bufferOffset      = 0,
        .bufferRowLength   = 0,
        .bufferImageHeight = 0,
        .imageSubresource  = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .mipLevel = 0, .baseArrayLayer = 0, .layerCount = 1},
        .imageOffset       = {0, 0, 0},
        .imageExtent       = {width, height, 1}};

    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    EndSingleTimeCommands(vkContext, commandBuffer);
}
VkImageView CreateImageView(VkDevice device, VkImage image, VkFormat format)
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
bool UploadBufferData(VulkanContext& vkContext, VkBuffer buffer, VkDeviceMemory bufferMemory, const void* data, VkDeviceSize size)
{
    void* mapped = nullptr;
    if (vkMapMemory(vkContext.device, bufferMemory, 0, size, 0, &mapped) != VK_SUCCESS)
    {
        return false;
    }

    std::memcpy(mapped, data, static_cast<size_t>(size));
    vkUnmapMemory(vkContext.device, bufferMemory);
    return true;
}
VkSampler CreateTextureSampler(VulkanContext& context)
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
std::vector<VkDescriptorSet> PrepareDrawCallDescriptorSets(VulkanContext& context, VkDescriptorPool pool,
                                                           const std::vector<VkDescriptorSetLayout>& layouts,
                                                           const PipelineConfig& pipelineConfig,
                                                           const std::map<uint32, VulkanShaderBuffer>& boundBuffers,
                                                           const std::map<uint32, IdType>& boundTextures)
{
    const auto& descriptorBindings = pipelineConfig.descriptorBindings;

    if (layouts.empty())
    {
        if (!boundBuffers.empty() || !boundTextures.empty())
        {
            LOG_ERROR << "Resources provided, but no descriptor layouts exist!";
        }

        return {};
    }

    if (pool == VK_NULL_HANDLE)
    {
        LOG_ERROR << "Invalid descriptor pool!";
        return {};
    }

    // Validate layouts.
    for (uint32 set = 0; set < layouts.size(); ++set)
    {
        if (layouts[set] == VK_NULL_HANDLE)
        {
            LOG_ERROR << "Invalid descriptor set layout: " << set;
            return {};
        }
    }

    // Find descriptor configuration by binding and type.
    // Binding must identify exactly one set for a given resource type.
    auto findBindingConfig = [&](uint32 binding, auto typePredicate)
    {
        auto found = descriptorBindings.end();

        for (auto it = descriptorBindings.begin(); it != descriptorBindings.end(); ++it)
        {
            if (it->binding != binding || !typePredicate(it->descriptorType))
            {
                continue;
            }

            if (found != descriptorBindings.end())
            {
                if (found->set != it->set || found->descriptorType != it->descriptorType ||
                    found->descriptorCount != it->descriptorCount)
                {
                    LOG_ERROR << "Ambiguous descriptor binding: " << binding;
                    return descriptorBindings.end();
                }
            }
            else
            {
                found = it;
            }
        }

        return found;
    };

    // Validate buffers.
    for (const auto& [binding, shaderBuffer] : boundBuffers)
    {
        auto configIt = findBindingConfig(
            binding, [](VkDescriptorType type)
            { return type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER || type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC; });

        if (configIt == descriptorBindings.end())
        {
            LOG_ERROR << "Buffer descriptor configuration not found. "
                      << "Binding: " << binding;
            return {};
        }

        if (configIt->set >= layouts.size())
        {
            LOG_ERROR << "Invalid descriptor set: " << configIt->set;
            return {};
        }

        if (configIt->descriptorCount != 1)
        {
            LOG_ERROR << "Buffer descriptorCount must be 1. Binding: " << binding;
            return {};
        }

        if (shaderBuffer.buffer == VK_NULL_HANDLE || shaderBuffer.size == 0)
        {
            LOG_ERROR << "Invalid shader buffer. Binding: " << binding;
            return {};
        }
    }

    // Validate textures.
    for (const auto& [binding, textureId] : boundTextures)
    {
        auto configIt =
            findBindingConfig(binding, [](VkDescriptorType type) { return type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; });

        if (configIt == descriptorBindings.end())
        {
            // LOG_WARN << "Texture descriptor configuration not found. "
            //          << "Binding: " << binding;
            continue;
        }

        if (configIt->set >= layouts.size())
        {
            LOG_ERROR << "Invalid descriptor set: " << configIt->set;
            return {};
        }

        if (configIt->descriptorCount != 1)
        {
            LOG_ERROR << "Texture descriptorCount must be 1. Binding: " << binding;
            return {};
        }

        auto textureIt = context.textures.find(textureId);

        if (textureIt == context.textures.end())
        {
            LOG_ERROR << "Texture not found: " << textureId;
            return {};
        }

        const auto& texture = textureIt->second;

        if (texture.imageView == VK_NULL_HANDLE || texture.sampler == VK_NULL_HANDLE)
        {
            LOG_ERROR << "Invalid texture resources: " << textureId;
            return {};
        }
    }

    // Allocate all descriptor sets.
    std::vector<VkDescriptorSet> descriptorSets(layouts.size());
    VkDescriptorSetAllocateInfo allocInfo{.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
                                          .descriptorPool     = pool,
                                          .descriptorSetCount = static_cast<uint32>(layouts.size()),
                                          .pSetLayouts        = layouts.data()};

    if (vkAllocateDescriptorSets(context.device, &allocInfo, descriptorSets.data()) != VK_SUCCESS)
    {
        LOG_ERROR << "Failed to allocate DrawCall descriptor sets!";
        return {};
    }

    // Prepare descriptor writes.
    std::vector<VkWriteDescriptorSet> descriptorWrites;
    std::vector<VkDescriptorBufferInfo> bufferInfos;
    std::vector<VkDescriptorImageInfo> imageInfos;

    descriptorWrites.reserve(boundBuffers.size() + boundTextures.size());

    bufferInfos.reserve(boundBuffers.size());
    imageInfos.reserve(boundTextures.size());

    // Write buffer descriptors.
    for (const auto& [binding, shaderBuffer] : boundBuffers)
    {
        auto configIt = findBindingConfig(
            binding, [](VkDescriptorType type)
            { return type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER or type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC; });

        const auto& config = *configIt;

        VkDescriptorBufferInfo bufferInfo{.buffer = shaderBuffer.buffer, .offset = 0, .range = shaderBuffer.size};
        bufferInfos.push_back(bufferInfo);

        VkWriteDescriptorSet descriptorWrite{.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                             .dstSet          = descriptorSets[config.set],
                                             .dstBinding      = binding,
                                             .dstArrayElement = 0,
                                             .descriptorCount = config.descriptorCount,
                                             .descriptorType  = config.descriptorType,
                                             .pBufferInfo     = &bufferInfos.back()};

        descriptorWrites.push_back(descriptorWrite);
    }

    // Write texture descriptors.
    for (const auto& [binding, textureId] : boundTextures)
    {
        auto configIt =
            findBindingConfig(binding, [](VkDescriptorType type) { return type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; });

        if (configIt == descriptorBindings.end())
        {
            // LOG_WARN << "Skipping texture not supported by pipeline. "
            //          << "Binding: " << binding;
            continue;
        }

        const auto& config  = *configIt;
        const auto& texture = context.textures.at(textureId);

        VkDescriptorImageInfo imageInfo{
            .sampler = texture.sampler, .imageView = texture.imageView, .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};

        imageInfos.push_back(imageInfo);

        VkWriteDescriptorSet descriptorWrite{.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                             .dstSet          = descriptorSets[config.set],
                                             .dstBinding      = binding,
                                             .dstArrayElement = 0,
                                             .descriptorCount = config.descriptorCount,
                                             .descriptorType  = config.descriptorType,
                                             .pImageInfo      = &imageInfos.back()};

        descriptorWrites.push_back(descriptorWrite);
    }

    if (not descriptorWrites.empty())
    {
        vkUpdateDescriptorSets(context.device, static_cast<uint32>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }

    return descriptorSets;
}
void ExecuteDrawCall(VulkanContext& vkContext, VkCommandBuffer commandBuffer, VkDescriptorPool descriptorPool,
                     const VulkanProgram& program, const VulkanDrawCall& drawCall)
{
    if (not ValidateDrawCallBufferBindings(vkContext, program, drawCall))
    {
        LOG_ERROR << "Skipping invalid DrawCall. Program: " << drawCall.programId;
        return;
    }

    // 1. Prepare descriptor set
    auto drawCallSets =
        PrepareDrawCallDescriptorSets(vkContext, descriptorPool, program.descriptorSetLayouts, program.pipelineConfig,
                                      drawCall.boundShaderBuffers, drawCall.boundTextures);

    if (drawCallSets.empty())
        return;

    // 2. Prepare dynamic offsets
    std::vector<uint32> dynamicOffsets;
    dynamicOffsets.reserve(drawCall.boundShaderBuffers.size());

    for (const auto& [binding, shaderBuffer] : drawCall.boundShaderBuffers)
    {
        if (shaderBuffer.type == BufferType::DynamicPerFrame)
        {
            dynamicOffsets.push_back(shaderBuffer.dynamicOffset);
        }
    }

    // 3. Bind descriptor set
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, program.layout, 0,
                            static_cast<uint32>(drawCallSets.size()), drawCallSets.data(),
                            static_cast<uint32>(dynamicOffsets.size()), dynamicOffsets.empty() ? nullptr : dynamicOffsets.data());

    // 4. Execute draw command
    std::visit(
        visitor{[&](const VulkanMeshDraw& meshDraw)
                {
                    const auto meshIt = vkContext.meshes.find(meshDraw.id);

                    if (meshIt == vkContext.meshes.end())
                        return;

                    const auto& mesh         = meshIt->second;
                    VkBuffer vertexBuffers[] = {mesh.vertexBuffer};
                    VkDeviceSize offsets[]   = {0};

                    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

                    if (mesh.useIndices && mesh.indexBuffer != VK_NULL_HANDLE)
                    {
                        vkCmdBindIndexBuffer(commandBuffer, mesh.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
                        vkCmdDrawIndexed(commandBuffer, static_cast<uint32>(mesh.meshData.indices_.size()), 1, 0, 0, 0);
                    }
                    else
                    {
                        vkCmdDraw(commandBuffer, mesh.vertexCount, 1, 0, 0);
                    }
                },
                [&](const VulkanProceduralDraw& proceduralDraw) { vkCmdDraw(commandBuffer, proceduralDraw.count, 1, 0, 0); }},
        drawCall.drawCommand);
}
void RecordDrawCalls(VulkanContext& vkContext, VkCommandBuffer commandBuffer, uint32 imageIndex)
{
    VkViewport viewport{};
    viewport.x        = 0.0f;
    viewport.y        = static_cast<float>(vkContext.swapChainExtent.height);
    viewport.width    = static_cast<float>(vkContext.swapChainExtent.width);
    viewport.height   = -static_cast<float>(vkContext.swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {.x = 0, .y = 0};
    scissor.extent = vkContext.swapChainExtent;

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    VkPipeline currentPipeline = VK_NULL_HANDLE;

    for (auto& [programId, program] : vkContext.programs)
    {
        if (imageIndex >= program.descriptorPools.size())
            continue;

        vkResetDescriptorPool(vkContext.device, program.descriptorPools[imageIndex], 0);
    }

    for (const auto& drawCall : vkContext.pendingDrawCalls)
    {
        auto programIt = vkContext.programs.find(drawCall.programId);

        if (programIt == vkContext.programs.end())
            continue;

        auto& program = programIt->second;

        if (imageIndex >= program.descriptorPools.size())
            continue;

        auto currentPool = program.descriptorPools[imageIndex];
        if (currentPipeline != program.pipeline)
        {
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, program.pipeline);

            currentPipeline = program.pipeline;
        }

        ExecuteDrawCall(vkContext, commandBuffer, currentPool, program, drawCall);
    }
}
bool EndRenderPass(VkCommandBuffer commandBuffer)
{
    vkCmdEndRenderPass(commandBuffer);
    return true;
}
bool EndCommandBuffer(VkCommandBuffer commandBuffer)
{
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        LOG_ERROR << "Error: Failed to finish recording the command buffer.\n";
        return false;
    }

    return true;
}
bool SubmitCommandBuffer(VulkanContext& vkContext, VkCommandBuffer commandBuffer)
{
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[]      = {vkContext.imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount     = 1;
    submitInfo.pWaitSemaphores        = waitSemaphores;
    submitInfo.pWaitDstStageMask      = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &commandBuffer;

    VkSemaphore signalSemaphores[]  = {vkContext.renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = signalSemaphores;

    if (vkQueueSubmit(vkContext.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
    {
        LOG_ERROR << "Error while submitting render commands to the queue.\n";
        return false;
    }

    return true;
}
ID CreateQuad(VulkanContext& vkContext)
{
    std::vector<float> vertices = {
        -1.0f, -1.0f,  // 0: Lewy dół
        1.0f,  -1.0f,  // 1: Prawy dół
        1.0f,  1.0f,   // 2: Prawy góra
        -1.0f, 1.0f    // 3: Lewy góra
    };

    std::vector<uint16_t> indices = {0, 1, 2, 0, 2, 3};

    VulkanMesh quadMesh{};
    quadMesh.vertexCount = 4;
    quadMesh.useIndices  = true;

    CreateBuffer(vkContext, vertices.size() * sizeof(float), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, quadMesh.vertexBuffer,
                 quadMesh.vertexBufferMemory);

    UploadBufferData(vkContext, quadMesh.vertexBuffer, quadMesh.vertexBufferMemory, vertices.data(),
                     vertices.size() * sizeof(float));

    CreateBuffer(vkContext, indices.size() * sizeof(uint16_t), VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, quadMesh.indexBuffer,
                 quadMesh.indexBufferMemory);

    UploadBufferData(vkContext, quadMesh.indexBuffer, quadMesh.indexBufferMemory, indices.data(),
                     indices.size() * sizeof(uint16_t));

    auto meshId              = vkContext.meshesPoolId.getId();
    vkContext.meshes[meshId] = std::move(quadMesh);

    LOG_DEBUG << "Quad created with ID: " << meshId;
    return meshId;
}
}  // namespace GraphicsApi::Vulkan
