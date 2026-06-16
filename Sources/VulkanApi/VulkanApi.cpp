#include "VulkanApi.h"

#include <Logger/Log.h>
#include <SDL2/SDL_vulkan.h>
#include <Variant.h>

#include <cstring>
#include <iostream>
#include <vector>

#include "PipelineConfig.h"
#include "Types.h"
#include "VulkanApi/SdlVulkanApi.h"
#include "VulkanApi/VulkanContext.hpp"
#include "VulkanApi/VulkanProgram.h"
#include "VulkanShaderCompiler.h"

namespace GraphicsApi::Vulkan
{
namespace
{
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
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = &vkContext.renderFinishedSemaphore;

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

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass      = vkContext.renderPass;
        framebufferInfo.attachmentCount = 2;
        framebufferInfo.pAttachments    = attachments;
        framebufferInfo.width           = vkContext.swapChainExtent.width;
        framebufferInfo.height          = vkContext.swapChainExtent.height;
        framebufferInfo.layers          = 1;

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
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = vkContext.graphicsFamilyIndex;
    poolInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(vkContext.device, &poolInfo, nullptr, &vkContext.commandPool) != VK_SUCCESS)
    {
        LOG_ERROR << "Error: Failed to create Command Pool.\n";
        return false;
    }

    vkContext.commandBuffers.resize(vkContext.framebuffers.size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool        = vkContext.commandPool;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32>(vkContext.commandBuffers.size());

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
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

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
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size        = size;
    bufferInfo.usage       = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

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
    vkAllocateCommandBuffers(vkContext.device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                                          .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT};

    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    return commandBuffer;
}

void EndSingleTimeCommands(VulkanContext& vkContext, VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

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

VkDescriptorSet PrepareDrawCallDescriptorSet(VulkanContext& context, VkDescriptorPool pool, VkDescriptorSetLayout layout,
                                             const std::map<uint32, uint32>& boundBuffers)
{
    VkDescriptorSet drawCallSet{VK_NULL_HANDLE};

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool     = pool;
    allocInfo.descriptorSetCount = 1u;
    allocInfo.pSetLayouts        = &layout;

    if (vkAllocateDescriptorSets(context.device, &allocInfo, &drawCallSet) != VK_SUCCESS)
    {
        LOG_ERROR << "Error: Failed to allocate dynamic VkDescriptorSet for DrawCall!\n";
        return VK_NULL_HANDLE;
    }

    std::vector<VkWriteDescriptorSet> descriptorWrites{};
    std::vector<VkDescriptorBufferInfo> bufferInfos{};
    bufferInfos.reserve(boundBuffers.size());

    for (auto const& [bindLocation, bufferId] : boundBuffers)
    {
        auto bufIt = context.shaderBuffers.find(bufferId);
        if (bufIt == context.shaderBuffers.end())
            continue;

        auto& shaderBuffer = bufIt->second;

        VkDescriptorBufferInfo bufferInfo{};

        if (shaderBuffer.type == BufferType::DynamicPerFrame)
        {
            // Dla dynamicznego UBO bierzemy globalny bufor (bo nasz wirtualny ma poprawnie przypisany uchwyt z dUbo w Create)
            bufferInfo.buffer = shaderBuffer.buffer;
            bufferInfo.offset = 0;  // BARDZO WAŻNE: Dla dynamicznych UBO offset w strukturze info zawsze wynosi 0!
            bufferInfo.range  = shaderBuffer.size;  // Rozmiar pojedynczego okienka danych
        }
        else
        {
            bufferInfo.buffer = shaderBuffer.buffer;
            bufferInfo.offset = 0;
            bufferInfo.range  = shaderBuffer.size;
        }

        bufferInfos.push_back(bufferInfo);

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet          = drawCallSet;
        descriptorWrite.dstBinding      = bindLocation;
        descriptorWrite.dstArrayElement = 0;

        // Zależnie od typu bufora ustawiamy odpowiedni typ deskryptora
        descriptorWrite.descriptorType = (shaderBuffer.type == BufferType::DynamicPerFrame)
                                             ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
                                             : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

        descriptorWrite.descriptorCount = 1;
        descriptorWrites.push_back(descriptorWrite);
    }

    for (auto i = size_t{0u}; i < descriptorWrites.size(); ++i)
    {
        descriptorWrites[i].pBufferInfo = &bufferInfos[i];
    }

    if (not descriptorWrites.empty())
    {
        vkUpdateDescriptorSets(context.device, static_cast<uint32>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }

    return drawCallSet;
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

    for (auto& [_, program] : vkContext.programs)
    {
        if (program.drawCalls.empty())
        {
            continue;
        }

        auto& currentPool = program.descriptorPools[imageIndex];
        vkResetDescriptorPool(vkContext.device, currentPool, 0);
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, program.pipeline);

        for (const auto& drawCall : program.drawCalls)
        {
            auto drawCallSet =
                PrepareDrawCallDescriptorSet(vkContext, currentPool, program.descriptorSetLayout, drawCall.boundShaderBuffers);

            if (drawCallSet == VK_NULL_HANDLE)
            {
                continue;
            }

            std::vector<uint32> dynamicOffsets;
            const uint32_t totalBindingsCount = 12;

            for (uint32_t i = 0; i < totalBindingsCount; ++i)
            {
                // Sprawdzamy naszą funkcją przejściową, czy ten slot w ogóle oczekuje typu Dynamic
                if (GetDrawFlagByBindLocation(i) == GraphicsApi::DrawFlag::Dynamic)
                {
                    uint32_t offsetForThisSlot = 0;  // Domyślnie bezpieczne zero

                    // Szukamy, czy silnik podpiął jakiś bufor pod ten konkretny bindLocation (i)
                    auto boundIt = drawCall.boundShaderBuffers.find(i);
                    if (boundIt != drawCall.boundShaderBuffers.end())
                    {
                        auto bufIt = vkContext.shaderBuffers.find(boundIt->second);
                        if (bufIt != vkContext.shaderBuffers.end() && bufIt->second.type == BufferType::DynamicPerFrame)
                        {
                            offsetForThisSlot = bufIt->second.dynamicOffset;
                        }
                    }
                    // LOG_DEBUG << "Slot " << i << " jest Dynamic. Dodaję offset: " << offsetForThisSlot;
                    //  Vulkan dostanie offset dokładnie w takiej kolejności, w jakiej ma zdefiniowane sloty dynamiczne
                    dynamicOffsets.push_back(offsetForThisSlot);
                }
            }

            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, program.layout, 0, 1, &drawCallSet,
                                    static_cast<uint32>(dynamicOffsets.size()),
                                    dynamicOffsets.empty() ? nullptr : dynamicOffsets.data());

            std::visit(
                visitor{[&](VulkanMeshDraw meshDraw)
                        {
                            const auto meshIt = vkContext.meshes.find(meshDraw.id);
                            if (meshIt == vkContext.meshes.end())
                            {
                                return;
                            }

                            VkBuffer vertexBuffers[] = {meshIt->second.vertexBuffer};
                            VkDeviceSize offsets[]   = {0};
                            vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

                            if (meshIt->second.useIndices and meshIt->second.indexBuffer != VK_NULL_HANDLE)
                            {
                                vkCmdBindIndexBuffer(commandBuffer, meshIt->second.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
                                vkCmdDrawIndexed(commandBuffer, static_cast<uint32>(meshIt->second.meshData.indices_.size()), 1,
                                                 0, 0, 0);
                            }
                            else
                            {
                                vkCmdDraw(commandBuffer, meshIt->second.vertexCount, 1, 0, 0);
                            }
                        },
                        [&](VulkanProceduralDraw proceduralDraw) { vkCmdDraw(commandBuffer, proceduralDraw.count, 1, 0, 0); }},
                drawCall.drawCommand);
        }

        program.drawCalls.clear();
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
}  // namespace

VulkanApi::VulkanApi()
    : windowApi_(std::make_unique<SdlVulkanApi>(vkContext))
    , shaderManager_(vkContext)
{
    LOG_DEBUG << "";
}

VulkanApi::~VulkanApi()
{
    DeleteContext();
}

IWindowApi& VulkanApi::GetWindowApi()
{
    return *windowApi_;
}

void VulkanApi::Init()
{
}

void VulkanApi::EndFrame()
{
    if (vkContext.swapChain == VK_NULL_HANDLE)
    {
        return;
    }

    uint32 imageIndex = 0;

    if (!AcquireNextSwapChainImage(vkContext, imageIndex))
    {
        RecreateSwapChain();
        return;
    }

    RenderFrame(imageIndex);

    if (!PresentSwapChainImage(vkContext, imageIndex))
    {
        RecreateSwapChain();
        return;
    }

    WaitForDeviceIdle(vkContext);
}

void VulkanApi::CreateContext()
{
    windowApi_->CreateContext();

    PrintVersion();

    if (!InitializeRenderingResources(vkContext))
    {
        return;
    }

    if (!quadMeshId.has_value())
    {
        // quadMeshId = CreateQuad(vkContext);
        MeshRawData quadMeshData{};
        quadMeshData.positions_  = {-1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f};
        quadMeshData.textCoords_ = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
        quadMeshData.indices_    = {0, 1, 2, 0, 2, 3};
        quadMeshId               = CreateMesh(quadMeshData, RenderType::TRIANGLES);
    }

    InitializeDynamicUniformBuffer(64 * 1024 * 1024);
}

void VulkanApi::RecreateSwapChain()
{
    vkDeviceWaitIdle(vkContext.device);

    CleanupSwapChainResources(vkContext);
    windowApi_->RecreateSwapChain();

    InitializeRenderingResources(vkContext);
    LOG_DEBUG << "Vulkan rendering resources recreated successfully.";
}

void VulkanApi::InitRendering()
{
    if (!InitializeRenderingResources(vkContext))
    {
        return;
    }
}

void VulkanApi::DeleteContext()
{
    vkContext.ClearResources();
}

void VulkanApi::PrintVersion()
{
    if (vkContext.physicalDevice == VK_NULL_HANDLE)
    {
        LOG_ERROR << "Vulkan device is not initialized.\n";
        return;
    }

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(vkContext.physicalDevice, &deviceProperties);
    LOG_DEBUG << "Vulkan device: " << deviceProperties.deviceName << "\n";
}

void VulkanApi::SetShadersFilesLocations(const std::filesystem::path& path)
{
    shaderManager_.SetShadersFilesLocations(path);
}
void VulkanApi::DebugNormalMeshGeneration(bool)
{
}
void VulkanApi::SetShaderQuaility(ShaderQuaility)
{
}
bool VulkanApi::IsTesselationSupported() const
{
    return false;
}
std::vector<RendererType> VulkanApi::GetSupportedRenderers() const
{
    return {RendererType::SIMPLE};
}
void VulkanApi::SetDefaultTarget()
{
}
void VulkanApi::SetBackgroundColor(const Color& color)
{
    vkContext.backgroundColor = color;
}
const Color& VulkanApi::GetBackgroundColor() const
{
    return vkContext.backgroundColor;
}
void VulkanApi::EnableDepthTest()
{
}
void VulkanApi::DisableDepthTest()
{
}
void VulkanApi::PrepareFrame()
{
    vkContext.globalDynamicUBO.currentOffset = 0;
}
ID VulkanApi::CreateShader(ShaderProgramType type)
{
    return shaderManager_.Create(type);
}
ID VulkanApi::CreateShaderBuffer(uint32 bindLocation, uint32 size, GraphicsApi::DrawFlag /*flag*/)
{
    auto flag = GetDrawFlagByBindLocation(bindLocation);
    if (flag == DrawFlag::Static)
    {
        VulkanShaderBuffer shaderBuffer{};
        shaderBuffer.size                = size;
        shaderBuffer.bindLocation        = bindLocation;
        shaderBuffer.type                = BufferType::StaticPerObject;
        VkBufferUsageFlags usage         = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        if (not CreateBuffer(vkContext, size, usage, properties, shaderBuffer.buffer, shaderBuffer.memory))
        {
            LOG_ERROR << "Error: Failed to create VkBuffer for UBO!\n";
            return {};
        }

        if (vkMapMemory(vkContext.device, shaderBuffer.memory, 0, size, 0, &shaderBuffer.mappedData) != VK_SUCCESS)
        {
            LOG_ERROR << "Error: Failed to map UBO memory to CPU space!\n";
            vkDestroyBuffer(vkContext.device, shaderBuffer.buffer, nullptr);
            vkFreeMemory(vkContext.device, shaderBuffer.memory, nullptr);
            return {};
        }

        const auto bufferId               = vkContext.shaderBuffersPoolId.getId();
        vkContext.shaderBuffers[bufferId] = std::move(shaderBuffer);

        vkContext.activeStaticAllocationsCount++;

        LOG_DEBUG << "Created Vulkan UBO ID: " << bufferId << ", size: " << size << " bytes"
                  << ", target bind location: " << bindLocation
                  << " | Active GPU allocations: " << vkContext.activeStaticAllocationsCount << "/4096";
        return bufferId;
    }

    VulkanShaderBuffer virtualBuffer{};
    virtualBuffer.size          = size;
    virtualBuffer.bindLocation  = bindLocation;
    virtualBuffer.type          = BufferType::DynamicPerFrame;
    virtualBuffer.dynamicOffset = 0;
    virtualBuffer.buffer        = vkContext.globalDynamicUBO.buffer;
    virtualBuffer.memory        = vkContext.globalDynamicUBO.memory;
    virtualBuffer.mappedData    = vkContext.globalDynamicUBO.mappedData;

    const auto virtualId               = vkContext.shaderBuffersPoolId.getId();
    vkContext.shaderBuffers[virtualId] = std::move(virtualBuffer);

    return virtualId;
}
ID VulkanApi::CreateShaderStorageBuffer(uint32, uint32, DrawFlag)
{
    return 0;
}
void VulkanApi::UpdateShaderBuffer(uint32 id, void const* buffer)
{
    auto it = vkContext.shaderBuffers.find(id);
    if (it == vkContext.shaderBuffers.end() || buffer == nullptr)
    {
        LOG_ERROR << "Error: Cannot update shader buffer. Invalid ID (" << id << ") or null data pointer!\n";
        return;
    }

    auto& shaderBuffer = it->second;

    if (shaderBuffer.type == BufferType::DynamicPerFrame)
    {
        auto& dUbo = vkContext.globalDynamicUBO;

        // 2. Wyliczamy wyrównany offset (alignment) dla obecnych danych
        uint32 alignedOffset = (dUbo.currentOffset + dUbo.alignment - 1) & ~(dUbo.alignment - 1);

        // 3. Sprawdzamy, czy dane zmieszczą się jeszcze w globalnym buforze
        if (alignedOffset + shaderBuffer.size > dUbo.totalSize)
        {
            LOG_ERROR << "Critical: Global Dynamic UBO Out of Memory! Resetting offset to 0. "
                      << "Requested: " << shaderBuffer.size << " bytes, Available: " << (dUbo.totalSize - alignedOffset)
                      << " bytes.";

            // W trybie jednoklatkowym awaryjnie resetujemy na 0 (może nadpisać dane, ale uchroni przed crashem)
            alignedOffset = 0;
        }

        // 4. Kopiujemy dane CPU do globalnego pasma pamięci pod wyliczony offset
        void* targetPtr = static_cast<char*>(dUbo.mappedData) + alignedOffset;
        std::memcpy(targetPtr, buffer, shaderBuffer.size);

        // 5. Zapisujemy wyliczony offset w tym KONKRETNYM obiekcie bufora.
        //    Dzięki temu podczas bindowania (vkCmdBindDescriptorSets) wiemy, jaki offset przekazać.
        shaderBuffer.dynamicOffset = alignedOffset;

        // 6. Przesuwamy globalny wskaźnik alokacji na koniec zapisanych danych
        dUbo.currentOffset = alignedOffset + shaderBuffer.size;
    }
    else
    {
        std::memcpy(shaderBuffer.mappedData, buffer, shaderBuffer.size);
    }
}
void VulkanApi::UpdateShaderStorageBuffer(uint32, void const*, uint32)
{
}
void* VulkanApi::MapShaderStorageBuffer(uint32, uint32, uint32)
{
    return nullptr;
}
void VulkanApi::UnmapShaderStorageBuffer(uint32)
{
}
uint32 VulkanApi::BindShaderBuffer(uint32 id)
{
    if (id == vkContext.globalDynamicUBOId)
    {
        LOG_WARN << "Warning: Trying to bind global UBO directly via ID " << id;
        return 0;
    }

    auto bufIt = vkContext.shaderBuffers.find(id);
    if (bufIt == vkContext.shaderBuffers.end())
    {
        LOG_ERROR << "Error: Cannot bind shader buffer. Buffer ID " << id << " not found!\n";
        return std::numeric_limits<uint32>::max();
    }

    const auto bindLocation                                       = bufIt->second.bindLocation;
    vkContext.currentRenderState.boundShaderBuffers[bindLocation] = id;
    return bufIt->second.bindLocation;
}
void VulkanApi::UseShader(uint32 shaderId)
{
    if (vkContext.programs.find(shaderId) == vkContext.programs.end())
    {
        LOG_ERROR << "Attempt to activate a non-existent shader/pipeline with ID: " << shaderId << "\n";
        return;
    }

    vkContext.currentRenderState.activeProgramId = shaderId;
}
ID VulkanApi::CreateTexture(const Utils::Image& image, [[maybe_unused]] TextureFilter filter,
                            [[maybe_unused]] TextureMipmap mipmap)
{
    // return {};

    VkDeviceSize imageSize = image.width * image.height * 4;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    CreateBuffer(vkContext, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(vkContext.device, stagingBufferMemory, 0, imageSize, 0, &data);
    std::memcpy(data, image.getRawDataPtr(), static_cast<size_t>(imageSize));
    vkUnmapMemory(vkContext.device, stagingBufferMemory);

    VulkanTexture texture;
    if (not CreateImage(vkContext, image.width, image.height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
                        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        texture.image, texture.memory))
    {
        vkDestroyBuffer(vkContext.device, stagingBuffer, nullptr);
        vkFreeMemory(vkContext.device, stagingBufferMemory, nullptr);
        return {};
    }

    TransitionImageLayout(vkContext, texture.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    CopyBufferToImage(vkContext, stagingBuffer, texture.image, static_cast<uint32>(image.width),
                      static_cast<uint32>(image.height));
    TransitionImageLayout(vkContext, texture.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(vkContext.device, stagingBuffer, nullptr);
    vkFreeMemory(vkContext.device, stagingBufferMemory, nullptr);

    texture.imageView = CreateImageView(vkContext.device, texture.image, VK_FORMAT_R8G8B8A8_SRGB);

    if (texture.imageView == VK_NULL_HANDLE)
    {
        return {};
    }

    const auto textureId          = vkContext.texturesPoolId.getId();
    vkContext.textures[textureId] = std::move(texture);

    return textureId;
}
ID VulkanApi::CreateTexture(const std::vector<Utils::Image>&, TextureFilter, TextureMipmap)
{
    return 0;
}
ID VulkanApi::CreateTextureStorage(TextureType, TextureFilter, int32)
{
    return 0;
}
ID VulkanApi::CreateCubMapTexture(const CubemapImages&)
{
    return 0;
}
void VulkanApi::GenerateMipmaps(IdType)
{
}
void VulkanApi::CopyTexture(IdType, IdType, uint32, uint32)
{
}
void VulkanApi::UpdateTexture(uint32, const vec2ui&, const Utils::Image&)
{
}
void VulkanApi::UpdateTexture(uint32, const Utils::Image&)
{
}
void VulkanApi::ClearTexture(uint32, const Color&)
{
}
void VulkanApi::EnableBlend()
{
}
void VulkanApi::DisableBlend()
{
}
void VulkanApi::EnableDepthMask()
{
}
void VulkanApi::DisableDepthMask()
{
}
void VulkanApi::ActiveTexture(uint32, uint32)
{
}
void VulkanApi::ActiveTexture(uint32)
{
}
void VulkanApi::DeleteObject(uint32)
{
}
void VulkanApi::DeleteObject(const std::vector<uint32>&)
{
}
void VulkanApi::DeleteShaderBuffer(uint32 id)
{
    auto it = vkContext.shaderBuffers.find(id);
    if (it == vkContext.shaderBuffers.end())
        return;

    if (it->second.type == BufferType::StaticPerObject)
    {
        vkUnmapMemory(vkContext.device, it->second.memory);
        vkDestroyBuffer(vkContext.device, it->second.buffer, nullptr);
        vkFreeMemory(vkContext.device, it->second.memory, nullptr);  // <--- Tutaj zwalniasz alokację!

        if (vkContext.activeStaticAllocationsCount > 0)
        {
            vkContext.activeStaticAllocationsCount--;
        }

        LOG_DEBUG << "Destroyed Static UBO ID: " << id << " | Active GPU allocations: " << vkContext.activeStaticAllocationsCount
                  << "/4096";
    }
    else
    {
        // LOG_DEBUG << "Destroyed Virtual Dynamic UBO ID: " << id << " (Global allocation untouched)";
    }

    vkContext.shaderBuffers.erase(it);
}
void VulkanApi::DeleteShaderBuffer(const std::vector<uint32>&)
{
}
std::string VulkanApi::GetBufferStatus()
{
    return "";
}
ID VulkanApi::CreatePatchMesh(const std::vector<float>&)
{
    return 0;
}
ID VulkanApi::CreatePurePatchMeshInstanced(uint32, uint32)
{
    return 0;
}
ID VulkanApi::CreateMesh(const MeshRawData& meshData, RenderType renderType)
{
    if (renderType != RenderType::TRIANGLES and renderType != RenderType::TRIAGNLE_STRIP)
        return {};

    VulkanMesh mesh{};
    mesh.meshData    = meshData;
    mesh.renderType  = renderType;
    mesh.vertexCount = static_cast<uint32>(meshData.positions_.size() / 3u);
    mesh.useIndices  = not meshData.indices_.empty();

    const auto meshId = vkContext.meshesPoolId.getId();

    if (not meshData.positions_.empty())
    {
        std::vector<float> interleavedVertices;
        interleavedVertices.reserve(mesh.vertexCount * 19u);

        for (uint32 i = 0; i < mesh.vertexCount; ++i)
        {
            // 1. Position (vec3) - Zawsze obowiązkowe
            const auto posOffset = i * 3u;
            interleavedVertices.push_back(meshData.positions_[posOffset]);
            interleavedVertices.push_back(meshData.positions_[posOffset + 1u]);
            interleavedVertices.push_back(meshData.positions_[posOffset + 2u]);

            // 2. TexCoord (vec2)
            const auto texOffset = i * 2u;
            if (texOffset + 1u < meshData.textCoords_.size())
            {
                interleavedVertices.push_back(meshData.textCoords_[texOffset]);
                interleavedVertices.push_back(meshData.textCoords_[texOffset + 1u]);
            }
            else
            {
                interleavedVertices.push_back(0.0f);
                interleavedVertices.push_back(0.0f);
            }

            // 3. Normal (vec3)
            const auto normOffset = i * 3u;
            if (normOffset + 2u < meshData.normals_.size())
            {
                interleavedVertices.push_back(meshData.normals_[normOffset]);
                interleavedVertices.push_back(meshData.normals_[normOffset + 1u]);
                interleavedVertices.push_back(meshData.normals_[normOffset + 2u]);
            }
            else
            {
                // Domyślna normalna skierowana w stronę kamery (Z+)
                interleavedVertices.push_back(0.0f);
                interleavedVertices.push_back(0.0f);
                interleavedVertices.push_back(1.0f);
            }

            // 4. Tangent (vec3)
            const auto tangOffset = i * 3u;
            if (tangOffset + 2u < meshData.tangents_.size())
            {
                interleavedVertices.push_back(meshData.tangents_[tangOffset]);
                interleavedVertices.push_back(meshData.tangents_[tangOffset + 1u]);
                interleavedVertices.push_back(meshData.tangents_[tangOffset + 2u]);
            }
            else
            {
                interleavedVertices.push_back(0.0f);
                interleavedVertices.push_back(0.0f);
                interleavedVertices.push_back(0.0f);
            }

            // 5. Weights (vec4)
            const auto weightOffset = i * 4u;
            if (weightOffset + 3u < meshData.bonesWeights_.size())
            {
                interleavedVertices.push_back(meshData.bonesWeights_[weightOffset]);
                interleavedVertices.push_back(meshData.bonesWeights_[weightOffset + 1u]);
                interleavedVertices.push_back(meshData.bonesWeights_[weightOffset + 2u]);
                interleavedVertices.push_back(meshData.bonesWeights_[weightOffset + 3u]);
            }
            else
            {
                interleavedVertices.push_back(0.0f);
                interleavedVertices.push_back(0.0f);
                interleavedVertices.push_back(0.0f);
                interleavedVertices.push_back(0.0f);
            }

            // 6. BoneIds (ivec4) - PUŁAPKA BITOWA!
            // Ponieważ interleavedVertices to wektor floatów, a shader oczekuje liczb całkowitych (SINT),
            // musimy skopiować surowe bity int32 do obszaru pamięci floata przy użyciu std::bit_cast.
            const auto boneOffset = i * 4u;
            if (boneOffset + 3u < meshData.joinIds_.size())
            {
                interleavedVertices.push_back(std::bit_cast<float>(meshData.joinIds_[boneOffset]));
                interleavedVertices.push_back(std::bit_cast<float>(meshData.joinIds_[boneOffset + 1u]));
                interleavedVertices.push_back(std::bit_cast<float>(meshData.joinIds_[boneOffset + 2u]));
                interleavedVertices.push_back(std::bit_cast<float>(meshData.joinIds_[boneOffset + 3u]));
            }
            else
            {
                interleavedVertices.push_back(0.0f);
                interleavedVertices.push_back(0.0f);
                interleavedVertices.push_back(0.0f);
                interleavedVertices.push_back(0.0f);
            }
        }

        if (not CreateBuffer(vkContext, interleavedVertices.size() * sizeof(interleavedVertices[0]),
                             VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mesh.vertexBuffer,
                             mesh.vertexBufferMemory))
        {
            return {};
        }

        if (not UploadBufferData(vkContext, mesh.vertexBuffer, mesh.vertexBufferMemory, interleavedVertices.data(),
                                 interleavedVertices.size() * sizeof(interleavedVertices[0])))
        {
            vkDestroyBuffer(vkContext.device, mesh.vertexBuffer, nullptr);
            vkFreeMemory(vkContext.device, mesh.vertexBufferMemory, nullptr);
            return {};
        }
    }

    if (not meshData.indices_.empty())
    {
        if (not CreateBuffer(vkContext, meshData.indices_.size() * sizeof(meshData.indices_[0]), VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mesh.indexBuffer,
                             mesh.indexBufferMemory))
        {
            return {};
        }

        if (not UploadBufferData(vkContext, mesh.indexBuffer, mesh.indexBufferMemory, meshData.indices_.data(),
                                 meshData.indices_.size() * sizeof(meshData.indices_[0])))
        {
            vkDestroyBuffer(vkContext.device, mesh.indexBuffer, nullptr);
            vkFreeMemory(vkContext.device, mesh.indexBufferMemory, nullptr);
            return {};
        }
    }

    vkContext.meshes[meshId] = std::move(mesh);

    LOG_DEBUG << "Created Vulkan mesh ID: " << meshId << ", vertices: " << vkContext.meshes[meshId].vertexCount
              << ", indices: " << vkContext.meshes[meshId].meshData.indices_.size();
    return meshId;
}
ID VulkanApi::CreateDynamicLineMesh()
{
    return 0;
}
ID VulkanApi::CreateParticle()
{
    return 0;
}
ID VulkanApi::CreateAnimatedParticle()
{
    return 0;
}
void VulkanApi::Compute(uint32, uint32, uint32)
{
}
void VulkanApi::RenderPurePatchedMeshInstances(uint32)
{
}
void VulkanApi::RenderMesh(uint32 meshId)
{
    if (vkContext.meshes.find(meshId) == vkContext.meshes.end())
    {
        LOG_ERROR << "Attempt to render a non-existent Vulkan mesh with ID: " << meshId << "\n";
        return;
    }

    auto it = vkContext.programs.find(vkContext.currentRenderState.activeProgramId);

    if (it != vkContext.programs.end())
    {
        VulkanDrawCall drawCall{.drawCommand        = VulkanMeshDraw{meshId},
                                .boundShaderBuffers = vkContext.currentRenderState.boundShaderBuffers};
        it->second.drawCalls.push_back(drawCall);
    }
    else
    {
        LOG_ERROR << "Error: The active shader ID (" << vkContext.currentRenderState.activeProgramId
                  << ") does not exist in the Vulkan context!\n";
    }
}
void VulkanApi::RenderProcedural(uint32 count)
{
    auto it = vkContext.programs.find(vkContext.currentRenderState.activeProgramId);

    if (it != vkContext.programs.end())
    {
        VulkanDrawCall drawCall{.drawCommand        = VulkanProceduralDraw{count},
                                .boundShaderBuffers = vkContext.currentRenderState.boundShaderBuffers};
        it->second.drawCalls.push_back(drawCall);
    }
}
void VulkanApi::RenderDebugNormals(uint32)
{
}
void VulkanApi::RenderTriangleStripMesh(uint32 id)
{
    RenderMesh(id);
}
void VulkanApi::RenderMeshInstanced(uint32 id, uint32 istanced)
{
}
void VulkanApi::RenderPoints(uint32)
{
}
void VulkanApi::RenderQuad()
{
    RenderMesh(quadMeshId.value_or(0));
}
void VulkanApi::RenderQuadTs()
{
    RenderQuad();
}
void VulkanApi::EnableClipingPlane(uint32)
{
}
void VulkanApi::DisableCliping(uint32)
{
}
void VulkanApi::EnableCulling()
{
}
void VulkanApi::DisableCulling()
{
}
void VulkanApi::SetViewPort(uint32, uint32, uint32, uint32)
{
}
void VulkanApi::BindTexture(uint32) const
{
}
void VulkanApi::BindImageTexture(uint32, TextureAccess)
{
}
ID VulkanApi::CreateShadowMap(uint32, uint32)
{
    return 0;
}
void VulkanApi::PolygonModeRender()
{
}
void VulkanApi::LineModeRender()
{
}
void VulkanApi::SetBlendFunction(BlendFunctionType)
{
}
void VulkanApi::UpdateMatrixes(uint32, const std::vector<mat4>&)
{
}
void VulkanApi::UpdateMesh(uint32, const MeshRawData&, const std::set<VertexBufferObjects>&)
{
}
void VulkanApi::UpdateLineMesh(uint32, const LineMesh&)
{
}
void VulkanApi::UpdateOffset(uint32, const std::vector<vec4>&)
{
}
void VulkanApi::UpdateBlend(uint32, const std::vector<float>&)
{
}
void VulkanApi::LoadViewMatrix(const mat4&)
{
}
void VulkanApi::LoadProjectionMatrix(const mat4&)
{
}
void VulkanApi::DrawLine(const vec3&, const vec3&, const vec3&)
{
}
mat4 VulkanApi::PrepareMatrixToLoad(const mat4& matrix)
{
    return matrix;
}
std::vector<uint8> VulkanApi::GetTextureData(uint32) const
{
    return {};
}
const TextureInfo& VulkanApi::GetTextureInfo(uint32) const
{
    return dummyTextureInfo;
}
void VulkanApi::TakeSnapshoot(const std::filesystem::path&) const
{
}
std::optional<Utils::Image> VulkanApi::GetImage(IdType) const
{
    return std::nullopt;
}
std::vector<Utils::Image> VulkanApi::GetImageArray(IdType) const
{
    return {};
}
IFrameBuffer& VulkanApi::GetDefaultFrameBuffer()
{
    return dummyFrameBuffer;
}
IFrameBuffer& VulkanApi::CreateFrameBuffer(const std::vector<FrameBuffer::Attachment>&)
{
    return dummyFrameBuffer;
}
void VulkanApi::DeleteFrameBuffer(IFrameBuffer&)
{
}
void VulkanApi::RenderFrame(uint32 imageIndex)
{
    VkCommandBuffer commandBuffer = vkContext.commandBuffers[imageIndex];

    if (!BeginCommandBuffer(commandBuffer))
    {
        return;
    }

    if (!BeginRenderPass(vkContext, commandBuffer, imageIndex))
    {
        return;
    }

    RecordDrawCalls(vkContext, commandBuffer, imageIndex);

    EndRenderPass(commandBuffer);

    if (!EndCommandBuffer(commandBuffer))
    {
        return;
    }

    SubmitCommandBuffer(vkContext, commandBuffer);
}
void VulkanApi::InitializeDynamicUniformBuffer(uint32 sizeInBytes)
{
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(vkContext.physicalDevice, &properties);

    auto& dUbo         = vkContext.globalDynamicUBO;
    dUbo.totalSize     = sizeInBytes;
    dUbo.currentOffset = 0;
    dUbo.alignment     = static_cast<uint32>(properties.limits.minUniformBufferOffsetAlignment);

    VkBufferUsageFlags usage       = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    VkMemoryPropertyFlags memProps = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    CreateBuffer(vkContext, dUbo.totalSize, usage, memProps, dUbo.buffer, dUbo.memory);
    vkMapMemory(vkContext.device, dUbo.memory, 0, dUbo.totalSize, 0, &dUbo.mappedData);

    vkContext.globalDynamicUBOId = vkContext.shaderBuffersPoolId.getId();

    LOG_DEBUG << "Initialized Global Dynamic UBO with total size: " << dUbo.totalSize << " bytes, alignment: " << dUbo.alignment
              << " bytes, and assigned ID: " << vkContext.globalDynamicUBOId;
}
}  // namespace GraphicsApi::Vulkan
