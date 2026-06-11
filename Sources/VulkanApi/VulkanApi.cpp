#include "VulkanApi.h"

#include <Logger/Log.h>
#include <SDL2/SDL_vulkan.h>

#include <cstring>
#include <iostream>
#include <vector>

#include "Types.h"
#include "VulkanApi/SdlVulkanApi.h"
#include "VulkanApi/VulkanContext.hpp"
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
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format         = vkContext.swapChainImageFormat;
    colorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments    = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments    = &colorAttachment;
    renderPassInfo.subpassCount    = 1;
    renderPassInfo.pSubpasses      = &subpass;

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
        VkImageView attachments[] = {vkContext.swapChainImageViews[i]};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass      = vkContext.renderPass;
        framebufferInfo.attachmentCount = 1;
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
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass        = vkContext.renderPass;
    renderPassInfo.framebuffer       = vkContext.framebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = vkContext.swapChainExtent;

    VkClearValue clearColor        = {{{vkContext.backgroundColor[0], vkContext.backgroundColor[1], vkContext.backgroundColor[2],
                                        vkContext.backgroundColor[3]}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues    = &clearColor;

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
                                             const std::unordered_map<uint32, uint32>& boundBuffers)
{
    auto drawCallSet = VkDescriptorSet{VK_NULL_HANDLE};

    auto allocInfo               = VkDescriptorSetAllocateInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool     = pool;
    allocInfo.descriptorSetCount = 1u;
    allocInfo.pSetLayouts        = &layout;

    if (vkAllocateDescriptorSets(context.device, &allocInfo, &drawCallSet) != VK_SUCCESS)
    {
        LOG_ERROR << "Error: Failed to allocate dynamic VkDescriptorSet for DrawCall!\n";
        return VK_NULL_HANDLE;
    }

    auto descriptorWrites = std::vector<VkWriteDescriptorSet>{};
    auto bufferInfos      = std::vector<VkDescriptorBufferInfo>{};
    bufferInfos.reserve(boundBuffers.size());

    for (auto const& [bindLocation, bufferId] : boundBuffers)
    {
        auto bufIt = context.shaderBuffers.find(bufferId);
        if (bufIt == context.shaderBuffers.end())
            continue;
        auto& shaderBuffer = bufIt->second;

        auto bufferInfo   = VkDescriptorBufferInfo{};
        bufferInfo.buffer = shaderBuffer.buffer;
        bufferInfo.offset = 0;
        bufferInfo.range  = shaderBuffer.size;
        bufferInfos.push_back(bufferInfo);

        auto descriptorWrite            = VkWriteDescriptorSet{};
        descriptorWrite.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet          = drawCallSet;
        descriptorWrite.dstBinding      = bindLocation;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
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
    scissor.offset = {
        .x = 0,
        .y = 0
    };
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
            const auto meshIt = vkContext.meshes.find(drawCall.meshId);
            if (meshIt == vkContext.meshes.end())
            {
                continue;
            }

            auto drawCallSet = PrepareDrawCallDescriptorSet(vkContext, currentPool, program.descriptorSetLayout,
                                                            drawCall.boundShaderBuffers);

            if (drawCallSet == VK_NULL_HANDLE)
            {
                continue;
            }

            VkBuffer vertexBuffers[] = {meshIt->second.vertexBuffer};
            VkDeviceSize offsets[]   = {0};
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, program.layout, 0, 1, &drawCallSet, 0,
                                    nullptr);

            if (meshIt->second.useIndices and meshIt->second.indexBuffer != VK_NULL_HANDLE)
            {
                vkCmdBindIndexBuffer(commandBuffer, meshIt->second.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
                vkCmdDrawIndexed(commandBuffer, static_cast<uint32>(meshIt->second.meshData.indices_.size()), 1, 0, 0, 0);
            }
            else
            {
                vkCmdDraw(commandBuffer, meshIt->second.vertexCount, 1, 0, 0);
            }
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
        MeshRawData quadMeshData{};
        quadMeshData.positions_  = {-1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f};
        quadMeshData.textCoords_ = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
        quadMeshData.indices_    = {0, 1, 2, 0, 2, 3};

        quadMeshId = CreateMesh(quadMeshData, RenderType::TRIANGLES);
    }
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
}
ID VulkanApi::CreateShader(ShaderProgramType type)
{
    return shaderManager_.Create(type);
}
ID VulkanApi::CreateShaderBuffer(uint32 bindLocation, uint32 size, GraphicsApi::DrawFlag flag)
{
    VulkanShaderBuffer shaderBuffer{};
    shaderBuffer.size                = size;
    shaderBuffer.bindLocation        = bindLocation;
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

    LOG_DEBUG << "Created Vulkan UBO ID: " << bufferId << ", size: " << size << " bytes"
              << ", target bind location: " << bindLocation;

    return bufferId;
}
ID VulkanApi::CreateShaderStorageBuffer(uint32, uint32, DrawFlag)
{
    return 0;
}
void VulkanApi::UpdateShaderBuffer(uint32 id, void const* buffer)
{
    auto it = vkContext.shaderBuffers.find(id);
    if (it == vkContext.shaderBuffers.end() or buffer == nullptr)
    {
        LOG_ERROR << "Error: Cannot update shader buffer. Invalid ID (" << id << ") or null data pointer!\n";
        return;
    }

    auto& shaderBuffer = it->second;
    std::memcpy(shaderBuffer.mappedData, buffer, shaderBuffer.size);
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
ID VulkanApi::CreateTexture(const Utils::Image&, TextureFilter, TextureMipmap)
{
    return 0;
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
void VulkanApi::DeleteShaderBuffer(uint32)
{
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
        VulkanDrawCall drawCall{.meshId = meshId, .boundShaderBuffers = vkContext.currentRenderState.boundShaderBuffers};
        it->second.drawCalls.push_back(drawCall);
    }
    else
    {
        LOG_ERROR << "Error: The active shader ID (" << vkContext.currentRenderState.activeProgramId
                  << ") does not exist in the Vulkan context!\n";
    }
}
void VulkanApi::RenderProcedural(uint32)
{
}
void VulkanApi::RenderDebugNormals(uint32)
{
}
void VulkanApi::RenderTriangleStripMesh(uint32)
{
}
void VulkanApi::RenderMeshInstanced(uint32, uint32)
{
}
void VulkanApi::RenderPoints(uint32)
{
}
void VulkanApi::RenderQuad()
{
    if (!quadMeshId.has_value())
    {
        MeshRawData quadMeshData{};
        quadMeshData.positions_  = {-1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f};
        quadMeshData.textCoords_ = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
        quadMeshData.indices_    = {0, 1, 2, 0, 2, 3};

        quadMeshId = CreateMesh(quadMeshData, RenderType::TRIANGLES);
    }

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
}  // namespace GraphicsApi::Vulkan
