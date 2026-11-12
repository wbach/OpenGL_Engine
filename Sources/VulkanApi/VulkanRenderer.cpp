#include "VulkanRenderer.h"

#include <Logger/Log.h>
#include <Utils/Variant.h>

#include "VulkanTextureManager.h"
#include "VulkanUtils.h"

namespace GraphicsApi::Vulkan
{
namespace
{
const auto isBufferDescriptorType(VkDescriptorType type)
{
    return type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER || type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC ||
           type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER || type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
};
template <typename TypePredicate>
const DescriptorBindingConfig* FindDescriptorBindingConfig(const std::vector<DescriptorBindingConfig>& bindings, uint32 binding,
                                                           TypePredicate&& typePredicate)
{
    const DescriptorBindingConfig* found = nullptr;

    for (const auto& config : bindings)
    {
        if (config.binding != binding || !typePredicate(config.descriptorType))
        {
            continue;
        }

        if (found == nullptr)
        {
            found = &config;
            continue;
        }

        if (found->set != config.set || found->descriptorType != config.descriptorType ||
            found->descriptorCount != config.descriptorCount)
        {
            return nullptr;
        }
    }

    return found;
}
}  // namespace
VulkanRenderer::VulkanRenderer(VulkanContext& context, VulkanTextureManager& textureManager)
    : context(context)
    , textureManager(textureManager)
{
}
void VulkanRenderer::PrepareFrame()
{
    pendingDrawCalls.clear();
    currentRenderState.boundTextures.fill(INVALID_ID);
    currentRenderState.boundShaderBuffers.fill(VulkanShaderBuffer{});
}
void VulkanRenderer::RenderFrame(ImageIndex imageIndex)
{
    VkCommandBuffer commandBuffer = context.commandBuffers[imageIndex];

    if (!BeginCommandBuffer(commandBuffer))
    {
        return;
    }

    if (!BeginRenderPass(commandBuffer, imageIndex))
    {
        return;
    }

    RecordDrawCalls(commandBuffer, imageIndex);

    EndRenderPass(commandBuffer);

    if (!EndCommandBuffer(commandBuffer))
    {
        return;
    }

    SubmitCommandBuffer(commandBuffer);
}
void VulkanRenderer::DrawCall(VulkanDrawCommand drawCommand)
{
    auto it = context.programs.find(currentRenderState.activeProgramId);

    if (it != context.programs.end())
    {
        // auto bufferBindings = createBufferBindingForDrawCall(it->second);

        pendingDrawCalls.push_back(VulkanDrawCall{.programId          = currentRenderState.activeProgramId,
                                                  .drawCommand        = drawCommand,
                                                  .boundShaderBuffers = currentRenderState.boundShaderBuffers,
                                                  .boundTextures      = currentRenderState.boundTextures});
    }
    else
    {
        LOG_ERROR << "Error: The active shader ID (" << currentRenderState.activeProgramId
                  << ") does not exist in the Vulkan context!\n";
    }
}
bool VulkanRenderer::BeginRenderPass(VkCommandBuffer commandBuffer, uint32 imageIndex)
{
    VkClearValue clearColor[2] = {
        {.color = VkClearColorValue{.float32 = {context.backgroundColor[0], context.backgroundColor[1],
                                                context.backgroundColor[2], context.backgroundColor[3]}}},
        {.depthStencil = VkClearDepthStencilValue{.depth = 1.0f, .stencil = 0}}};

    VkRenderPassBeginInfo renderPassInfo{.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                                         .renderPass      = context.renderPass,
                                         .framebuffer     = context.framebuffers[imageIndex],
                                         .renderArea      = VkRect2D{.offset = {0, 0}, .extent = context.swapChainExtent},
                                         .clearValueCount = 2,
                                         .pClearValues    = clearColor};

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    return true;
}
std::vector<VkDescriptorSet> VulkanRenderer::PrepareDrawCallDescriptorSets(VkDescriptorPool pool,
                                                                           const std::vector<VkDescriptorSetLayout>& layouts,
                                                                           const PipelineConfig& pipelineConfig,
                                                                           const VulkanShaderBufferBindings& boundBuffers,
                                                                           const VulkanTextureBindings& boundTextures)
{
    const auto& descriptorBindings = pipelineConfig.descriptorBindings;
    const auto& textures           = textureManager.GetTextures();

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
            if (it->binding != binding or not typePredicate(it->descriptorType))
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
    uint32 bindingIter = 0;
    for (const auto& shaderBuffer : boundBuffers)
    {
        auto binding = bindingIter++;

        if (shaderBuffer.size == 0)
            continue;

        auto configIt = findBindingConfig(binding, isBufferDescriptorType);

        if (configIt == descriptorBindings.end())
        {
            continue;
        }

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
    bindingIter = 0;
    for (const auto& textureId : boundTextures)
    {
        auto binding = bindingIter++;
        if (textureId == INVALID_ID)
            continue;

        auto configIt =
            findBindingConfig(binding, [](VkDescriptorType type) { return type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; });

        if (configIt == descriptorBindings.end())
        {
            continue;
        }

        const auto& config  = *configIt;
        const auto& texture = textures.at(textureId);

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
void VulkanRenderer::ExecuteDrawCall(VkCommandBuffer commandBuffer, VkDescriptorPool descriptorPool, const VulkanProgram& program,
                                     const VulkanDrawCall& drawCall)
{
    // 1. Prepare descriptor set
    auto drawCallSets = PrepareDrawCallDescriptorSets(descriptorPool, program.descriptorSetLayouts, program.pipelineConfig,
                                                      drawCall.boundShaderBuffers, drawCall.boundTextures);

    if (drawCallSets.empty())
        return;

    // 2. Prepare dynamic offsets
    std::vector<uint32> dynamicOffsets;
    dynamicOffsets.reserve(drawCall.boundShaderBuffers.size());

    for (const auto& shaderBuffer : drawCall.boundShaderBuffers)
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
                    const auto meshIt = context.meshes.find(meshDraw.id);

                    if (meshIt == context.meshes.end())
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
void VulkanRenderer::RecordDrawCalls(VkCommandBuffer commandBuffer, uint32 imageIndex)
{
    VkViewport viewport{.x        = 0.0f,
                        .y        = static_cast<float>(context.swapChainExtent.height),
                        .width    = static_cast<float>(context.swapChainExtent.width),
                        .height   = -static_cast<float>(context.swapChainExtent.height),
                        .minDepth = 0.0f,
                        .maxDepth = 1.0f};

    VkRect2D scissor{.offset = {.x = 0, .y = 0}, .extent = context.swapChainExtent};

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    VkPipeline currentPipeline = VK_NULL_HANDLE;

    for (auto& [programId, program] : context.programs)
    {
        if (imageIndex >= program.descriptorPools.size())
            continue;

        vkResetDescriptorPool(context.device, program.descriptorPools[imageIndex], 0);
    }

    for (auto& drawCall : pendingDrawCalls)
    {
        auto programIt = context.programs.find(drawCall.programId);

        if (programIt == context.programs.end())
            continue;

        auto& program = programIt->second;

        if (program.descriptorSetLayouts.empty())
            continue;
        if (imageIndex >= program.descriptorPools.size())
            continue;

        FilterDrawCallForProgram(program, drawCall);

        auto currentPool = program.descriptorPools[imageIndex];
        if (currentPipeline != program.pipeline)
        {
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, program.pipeline);
            currentPipeline = program.pipeline;
        }

        ExecuteDrawCall(commandBuffer, currentPool, program, drawCall);
    }
}
bool VulkanRenderer::EndRenderPass(VkCommandBuffer commandBuffer)
{
    vkCmdEndRenderPass(commandBuffer);
    return true;
}

bool VulkanRenderer::SubmitCommandBuffer(VkCommandBuffer commandBuffer)
{
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[]      = {context.imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount     = 1;
    submitInfo.pWaitSemaphores        = waitSemaphores;
    submitInfo.pWaitDstStageMask      = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &commandBuffer;

    VkSemaphore signalSemaphores[]  = {context.renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = signalSemaphores;

    if (vkQueueSubmit(context.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
    {
        LOG_ERROR << "Error while submitting render commands to the queue.\n";
        return false;
    }

    return true;
}
void VulkanRenderer::FilterDrawCallForProgram(const VulkanProgram& program, VulkanDrawCall& drawCall) const
{
    const auto& descriptorBindings = program.pipelineConfig.descriptorBindings;
    const auto& descriptorSetLayouts = program.descriptorSetLayouts;

    for (uint32 binding = 0; binding < drawCall.boundShaderBuffers.size(); ++binding)
    {
        auto& shaderBuffer = drawCall.boundShaderBuffers[binding];

        if (shaderBuffer.size == 0)
            continue;

        const auto descriptorType = GetDescriptorType(shaderBuffer.type);

        if (shaderBuffer.buffer == VK_NULL_HANDLE || !isBufferDescriptorType(descriptorType))
        {
            shaderBuffer = {};
            continue;
        }

        const auto* config = FindDescriptorBindingConfig(descriptorBindings, binding, isBufferDescriptorType);

        if (config == nullptr || config->set >= descriptorSetLayouts.size() || config->descriptorCount != 1 ||
            config->descriptorType != descriptorType)
        {
            shaderBuffer = {};
        }
    }

    for (uint32 binding = 0; binding < drawCall.boundTextures.size(); ++binding)
    {
        auto& textureId = drawCall.boundTextures[binding];

        if (textureId == INVALID_ID)
            continue;

        const auto* config = FindDescriptorBindingConfig(
            descriptorBindings, binding, [](VkDescriptorType type) { return type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; });

        if (config == nullptr || config->set >= descriptorSetLayouts.size() || config->descriptorCount != 1)
        {
            textureId = INVALID_ID;
        }
    }
}
}  // namespace GraphicsApi::Vulkan
