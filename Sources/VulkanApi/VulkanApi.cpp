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
#include "VulkanUtils.h"

namespace GraphicsApi::Vulkan
{
VulkanApi::VulkanApi()
    : windowApi_(std::make_unique<SdlVulkanApi>(vkContext))
    , bufferManager_(vkContext)
    , shaderManager_(vkContext)
    , textureManager_(vkContext, bufferManager_)
    , vulkanRenderer_(vkContext, textureManager_)
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
        quadMeshId =
            CreateMesh(MeshRawData{.positions_  = {-1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f},
                                   .textCoords_ = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f},
                                   .indices_    = {0, 1, 2, 0, 2, 3}},
                       RenderType::TRIANGLES);
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
    vkContext.pendingDrawCalls.clear();
    vkContext.currentRenderState.boundTextures.clear();
    vkContext.currentRenderState.boundShaderBuffers.clear();
    vkContext.globalDynamicUBO.currentOffset = 0;
}
ID VulkanApi::CreateShader(ShaderProgramType type)
{
    return shaderManager_.Create(type);
}
ID VulkanApi::CreateShaderBuffer(uint32 bindLocation, uint32 size, GraphicsApi::DrawFlag flag)
{
    // auto flag = GetDrawFlagByBindLocation(bindLocation);
    if (flag == DrawFlag::Static)
    {
        VulkanShaderBuffer shaderBuffer{.type = BufferType::StaticPerObject, .size = size, .bindLocation = bindLocation};

        VkBufferUsageFlags usage         = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        if (not bufferManager_.CreateBuffer(size, usage, properties, shaderBuffer.buffer, shaderBuffer.memory))
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

        LOG_DEBUG << "Created Vulkan Static UBO ID: " << bufferId << ", size: " << size << " bytes"
                  << ", target bind location: " << bindLocation
                  << " | Active GPU allocations: " << vkContext.activeStaticAllocationsCount << "/4096";
        return bufferId;
    }

    VulkanShaderBuffer virtualBuffer{.type          = BufferType::DynamicPerFrame,
                                     .buffer        = vkContext.globalDynamicUBO.buffer,
                                     .memory        = vkContext.globalDynamicUBO.memory,
                                     .mappedData    = vkContext.globalDynamicUBO.mappedData,
                                     .size          = size,
                                     .bindLocation  = bindLocation,
                                     .dynamicOffset = 0};

    const auto virtualId               = vkContext.shaderBuffersPoolId.getId();
    vkContext.shaderBuffers[virtualId] = std::move(virtualBuffer);

    LOG_DEBUG << "Created Vulkan Dynamic UBO ID: " << virtualId << ", size: " << size << " bytes"
              << ", target bind location: " << bindLocation;

    return virtualId;
}
ID VulkanApi::CreateShaderStorageBuffer(uint32, uint32, DrawFlag)
{
    LOG_ERROR << "Error: Shader Storage Buffer creation is not implemented in Vulkan backend!\n";
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
            return;
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
        // LOG_ERROR << "Error: Cannot bind shader buffer. Buffer ID " << id << " not found!\n";
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
        // LOG_ERROR << "Attempt to activate a non-existent shader/pipeline with ID: " << shaderId << "\n";
        return;
    }

    vkContext.currentRenderState.activeProgramId = shaderId;
}

GraphicsApi::ID VulkanApi::CreateTexture(const Utils::Image& image, TextureFilter filter, TextureMipmap mipmap)
{
    return textureManager_.CreateTexture(image, filter, mipmap);
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
void VulkanApi::ActiveTexture(uint32 unit, uint32 textureId)
{
    vkContext.currentRenderState.activeTextureUnit = unit;

    if (textureId == 0u)
    {
        vkContext.currentRenderState.boundTextures.erase(unit);
        return;
    }

    vkContext.currentRenderState.boundTextures[unit] = textureId;
}
void VulkanApi::ActiveTexture(uint32 unit)
{
    vkContext.currentRenderState.activeTextureUnit = unit;
}
void VulkanApi::BindTexture(uint32 textureId) const
{
    if (textureId == 0u)
        return;

    vkContext.currentRenderState.boundTextures[vkContext.currentRenderState.activeTextureUnit] = textureId;
}
void VulkanApi::BindImageTexture(uint32 textureId, TextureAccess)
{
    if (textureId == 0u)
        return;

    vkContext.currentRenderState.boundTextures[vkContext.currentRenderState.activeTextureUnit] = textureId;
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

        if (not bufferManager_.CreateBuffer(interleavedVertices.size() * sizeof(interleavedVertices[0]),
                                            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                            mesh.vertexBuffer, mesh.vertexBufferMemory))
        {
            return {};
        }

        if (not bufferManager_.UploadBufferData(mesh.vertexBuffer, mesh.vertexBufferMemory, interleavedVertices.data(),
                                                interleavedVertices.size() * sizeof(interleavedVertices[0])))
        {
            vkDestroyBuffer(vkContext.device, mesh.vertexBuffer, nullptr);
            vkFreeMemory(vkContext.device, mesh.vertexBufferMemory, nullptr);
            return {};
        }
    }

    if (not meshData.indices_.empty())
    {
        if (not bufferManager_.CreateBuffer(meshData.indices_.size() * sizeof(meshData.indices_[0]),
                                            VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                            mesh.indexBuffer, mesh.indexBufferMemory))
        {
            return {};
        }

        if (not bufferManager_.UploadBufferData(mesh.indexBuffer, mesh.indexBufferMemory, meshData.indices_.data(),
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

std::map<uint32, VulkanShaderBuffer> VulkanApi::createBufferBindingForDrawCall(const VulkanProgram& program,
                                                                               const RenderState& renderState)
{
    std::map<uint32, VulkanShaderBuffer> result;
    const auto& descriptorBindings = program.pipelineConfig.descriptorBindings;

    for (const auto& [bindLocation, bufferId] : renderState.boundShaderBuffers)
    {
        auto descriptorIt = std::find_if(descriptorBindings.begin(), descriptorBindings.end(),
                                         [bindLocation](const auto& binding) { return binding.binding == bindLocation; });

        if (descriptorIt == descriptorBindings.end())
        {
            // LOG_DEBUG << "Skipping bind location " << bindLocation << " as it is not part of the program's descriptor
            // bindings.";
            continue;
        }

        if (descriptorIt->descriptorType != VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER &&
            descriptorIt->descriptorType != VK_DESCRIPTOR_TYPE_STORAGE_BUFFER &&
            descriptorIt->descriptorType != VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC &&
            descriptorIt->descriptorType != VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC)
        {
            continue;
        }

        auto bufferIt = vkContext.shaderBuffers.find(bufferId);

        if (bufferIt == vkContext.shaderBuffers.end())
        {
            // LOG_WARN << "Warning: Shader buffer with ID " << bufferId << " not found for bind location " << bindLocation
            //          << ". Skipping this binding.\n";
            continue;
        }

        if (GetDescriptorType(bufferIt->second.type) != descriptorIt->descriptorType)
        {
            // LOG_ERROR << "Error: Shader buffer type mismatch for bind location " << bindLocation
            //           << ". Expected descriptor type: " << static_cast<uint32>(descriptorIt->descriptorType)
            //           << ", but got buffer type: " << static_cast<uint32>(GetDescriptorType(bufferIt->second.type)) << ".\n";
            return {};
        }

        result.emplace(bindLocation, bufferIt->second);
    }
    return result;
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
        VulkanDrawCall drawCall{.programId          = vkContext.currentRenderState.activeProgramId,
                                .drawCommand        = VulkanMeshDraw{meshId},
                                .boundShaderBuffers = createBufferBindingForDrawCall(it->second, vkContext.currentRenderState),
                                .boundTextures      = vkContext.currentRenderState.boundTextures};

        if (not drawCall.boundShaderBuffers.empty())
        {
            vkContext.pendingDrawCalls.push_back(drawCall);
        }
        else
        {
            // LOG_WARN << "Warning: No shader buffers bound for the active program ID ("
            //          << vkContext.currentRenderState.activeProgramId << "). Draw call will be skipped.\n";
        }
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
        VulkanDrawCall drawCall{.programId          = vkContext.currentRenderState.activeProgramId,
                                .drawCommand        = VulkanProceduralDraw{count},
                                .boundShaderBuffers = createBufferBindingForDrawCall(it->second, vkContext.currentRenderState),
                                .boundTextures      = vkContext.currentRenderState.boundTextures};
        if (not drawCall.boundShaderBuffers.empty())
        {
            vkContext.pendingDrawCalls.push_back(drawCall);
        }
        else
        {
            // LOG_WARN << "Warning: No shader buffers bound for the active program ID ("
            //          << vkContext.currentRenderState.activeProgramId << "). Draw call will be skipped.\n";
        }
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
void VulkanApi::TakeSnapshot(const std::filesystem::path&) const
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
   vulkanRenderer_.RenderFrame(imageIndex);
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

    bufferManager_.CreateBuffer(dUbo.totalSize, usage, memProps, dUbo.buffer, dUbo.memory);
    vkMapMemory(vkContext.device, dUbo.memory, 0, dUbo.totalSize, 0, &dUbo.mappedData);

    vkContext.globalDynamicUBOId = vkContext.shaderBuffersPoolId.getId();

    LOG_DEBUG << "Initialized Global Dynamic UBO with total size: " << dUbo.totalSize << " bytes, alignment: " << dUbo.alignment
              << " bytes, and assigned ID: " << vkContext.globalDynamicUBOId;
}
std::optional<uint64_t> VulkanApi::GetBindlessHandle(IdType)
{
    return {};
}
GraphicsApi::ClipSpaceOrigin VulkanApi::GetClipSpaceOrigin() const
{
    return GraphicsApi::ClipSpaceOrigin::UpperLeft;
}
GraphicsApi::ClipSpaceDepth VulkanApi::GetClipSpaceDepth() const
{
    return GraphicsApi::ClipSpaceDepth::ZeroToOne;
}

}  // namespace GraphicsApi::Vulkan
