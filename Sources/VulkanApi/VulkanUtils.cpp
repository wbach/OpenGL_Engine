#include "VulkanUtils.h"

#include <Logger/Log.h>
#include <Utils/Variant.h>

namespace GraphicsApi::Vulkan
{
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
bool EndCommandBuffer(VkCommandBuffer commandBuffer)
{
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        LOG_ERROR << "Error: Failed to finish recording the command buffer.\n";
        return false;
    }

    return true;
}
}  // namespace GraphicsApi::Vulkan
