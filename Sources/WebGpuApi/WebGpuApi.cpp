#include "WebGpuApi.h"

#include <Logger/Log.h>
#include <emscripten/html5.h>
#include <webgpu/webgpu.h>

#include <cstring>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "DefaultFrameBuffer.h"
#include "WebGpuContext.h"
#include "WebGpuFrameBufferManager.h"
#include "WebGpuMeshManager.h"
#include "WebGpuShaderManager.h"
#include "WebGpuTextureManager.h"
#include "WebGpuTriangleRenderer.h"
#include "WebGpuWindowApi.h"

namespace GraphicsApi::WebGpu
{
struct WebGpuShaderBufferResource
{
    WGPUBuffer buffer = nullptr;
    uint32 bindLocation = 0;
    uint32 size = 0;
    bool isStorage = false;
    std::vector<uint8_t> hostData;
};

namespace
{
struct WebGpuDrawCall
{
    uint32 programId = 0;
    uint32 meshId = 0;
    uint32 proceduralVertexCount = 0;
    bool isProcedural = false;
    std::unordered_map<uint32, uint32> boundShaderBuffers;
    std::unordered_map<uint32, uint32> activeTextureAssignments;
};

WGPUBindGroup CreateBindGroupForState(const WebGpuShaderProgram& program, WGPUDevice device,
                                      const std::unordered_map<uint32, WebGpuShaderBufferResource>& shaderBuffers,
                                      const std::unordered_map<uint32, uint32>& boundShaderBuffers,
                                      const std::unordered_map<uint32, uint32>& activeTextureAssignments,
                                      const WebGpuTextureManager& textureManager)
{
    if (!program.bindGroupLayout || !device || program.requiredBindings.empty())
    {
        return nullptr;
    }

    std::vector<WGPUBindGroupEntry> entries;
    entries.reserve(program.requiredBindings.size());

    for (const auto& requiredBinding : program.requiredBindings)
    {
        const auto binding = requiredBinding.binding;

        if (requiredBinding.buffer.type != WGPUBufferBindingType_Undefined)
        {
            const auto bufferIt = boundShaderBuffers.find(binding);
            if (bufferIt == boundShaderBuffers.end())
            {
                continue;
            }

            const auto shaderBufferIt = shaderBuffers.find(bufferIt->second);
            if (shaderBufferIt == shaderBuffers.end() || !shaderBufferIt->second.buffer)
            {
                return nullptr;
            }

            WGPUBindGroupEntry entry{};
            entry.binding = binding;
            entry.buffer = shaderBufferIt->second.buffer;
            entry.offset = 0;
            entry.size = shaderBufferIt->second.size;
            entries.push_back(entry);
            continue;
        }

        if (requiredBinding.texture.sampleType != WGPUTextureSampleType_Undefined)
        {
            const auto textureIt = activeTextureAssignments.find(binding);
            if (textureIt == activeTextureAssignments.end() || textureIt->second == 0)
            {
                continue;
            }

            const auto textureView = textureManager.GetTextureView(textureIt->second);
            if (!textureView)
            {
                continue;
            }

            WGPUBindGroupEntry textureEntry{};
            textureEntry.binding = binding;
            textureEntry.textureView = *textureView;
            entries.push_back(textureEntry);
            continue;
        }

        if (requiredBinding.sampler.type != WGPUSamplerBindingType_Undefined)
        {
            const auto textureIt = activeTextureAssignments.find(binding - 1u);
            if (textureIt == activeTextureAssignments.end() || textureIt->second == 0)
            {
                continue;
            }

            const auto sampler = textureManager.GetSampler(textureIt->second);
            if (!sampler)
            {
                continue;
            }

            WGPUBindGroupEntry samplerEntry{};
            samplerEntry.binding = binding;
            samplerEntry.sampler = *sampler;
            entries.push_back(samplerEntry);
        }
    }

    if (entries.empty())
    {
        return nullptr;
    }

    WGPUBindGroupDescriptor descriptor{};
    descriptor.layout = program.bindGroupLayout;
    descriptor.entryCount = static_cast<uint32_t>(entries.size());
    descriptor.entries = entries.data();
    return wgpuDeviceCreateBindGroup(device, &descriptor);
}

void DrawQueuedMesh(const WebGpuShaderManager& shaderManager, const WebGpuMeshManager& meshManager,
                    WGPUDevice device, WGPURenderPassEncoder renderPass, const WebGpuDrawCall& drawCall,
                    const std::unordered_map<uint32, WebGpuShaderBufferResource>& shaderBuffers,
                    const WebGpuTextureManager& textureManager)
{
    const auto* program = shaderManager.GetProgram(drawCall.programId);
    if (!program || !program->pipeline)
    {
        return;
    }

    wgpuRenderPassEncoderSetPipeline(renderPass, program->pipeline);

    WGPUBindGroup bindGroup = CreateBindGroupForState(*program, device, shaderBuffers,
                                                     drawCall.boundShaderBuffers, drawCall.activeTextureAssignments,
                                                     textureManager);
    if (bindGroup)
    {
        wgpuRenderPassEncoderSetBindGroup(renderPass, 0, bindGroup, 0, nullptr);
    }

    if (drawCall.isProcedural)
    {
        wgpuRenderPassEncoderDraw(renderPass, drawCall.proceduralVertexCount, 1, 0, 0);
        if (bindGroup)
        {
            wgpuBindGroupRelease(bindGroup);
        }
        return;
    }

    const auto* mesh = meshManager.GetMeshResource(drawCall.meshId);
    if (!mesh || !mesh->vertexBuffer)
    {
        if (bindGroup)
        {
            wgpuBindGroupRelease(bindGroup);
        }
        return;
    }

    const uint64_t vertexStrideBytes = sizeof(float) * mesh->vertexStride * mesh->vertexCount;
    if (mesh->hasIndices && mesh->indexBuffer)
    {
        wgpuRenderPassEncoderSetVertexBuffer(renderPass, 0, mesh->vertexBuffer, 0, vertexStrideBytes);
        wgpuRenderPassEncoderSetIndexBuffer(renderPass, mesh->indexBuffer, WGPUIndexFormat_Uint32, 0,
                                            mesh->indexCount * sizeof(uint32_t));
        wgpuRenderPassEncoderDrawIndexed(renderPass, mesh->indexCount, 1, 0, 0, 0);
    }
    else
    {
        wgpuRenderPassEncoderSetVertexBuffer(renderPass, 0, mesh->vertexBuffer, 0, vertexStrideBytes);
        wgpuRenderPassEncoderDraw(renderPass, mesh->vertexCount, 1, 0, 0);
    }

    if (bindGroup)
    {
        wgpuBindGroupRelease(bindGroup);
    }
}

void RebuildBindGroupForCurrentProgram(WebGpuShaderManager& shaderManager, WGPUDevice device,
                                      const std::unordered_map<uint32, WebGpuShaderBufferResource>& shaderBuffers,
                                      std::unordered_map<uint32, uint32>& boundShaderBuffers,
                                      const WebGpuTextureManager& textureManager)
{
    const auto currentProgramId = shaderManager.GetCurrentProgramId();
    if (currentProgramId == 0)
    {
        return;
    }

    auto* program = shaderManager.GetProgram(currentProgramId);
    if (!program || !program->bindGroupLayout || !device)
    {
        return;
    }

    if (program->bindGroup)
    {
        wgpuBindGroupRelease(program->bindGroup);
        program->bindGroup = nullptr;
    }

    program->bindGroup = CreateBindGroupForState(*program, device, shaderBuffers, boundShaderBuffers,
                                                textureManager.GetActiveTextureAssignments(), textureManager);
}
}  // namespace

struct WebGpuApi::Impl
{
    std::unique_ptr<IWindowApi> windowApi;
    WebGpuContext context;
    WebGpuTriangleRenderer triangleRenderer;
    WebGpuTextureManager textureManager;
    WebGpuMeshManager meshManager;
    WebGpuShaderManager shaderManager;
    WebGpuFrameBufferManager frameBufferManager;
    std::vector<WebGpuDrawCall> queuedDrawCalls;
    mat4 viewMatrix = mat4(1.f);
    mat4 projectionMatrix = mat4(1.f);

    uint32 nextShaderBufferId = 1;
    std::unordered_map<uint32, WebGpuShaderBufferResource> shaderBuffers;
    std::unordered_map<uint32, uint32> boundShaderBuffers;
    std::optional<uint32> quadMeshId;

    Impl()
        : windowApi{std::make_unique<WebGpuWindowApi>()}
    {
    }
};

WebGpuApi::WebGpuApi()
    : impl_{std::make_unique<Impl>()}
{
}
WebGpuApi::~WebGpuApi()
{
}
void WebGpuApi::CreateContextAsync(ContextReadyCallback callback)
{
    if (impl_->context.IsCreationStarted())
    {
        LOG_ERROR << "WebGPU context initialization was already started.";

        if (callback)
            callback(false);

        return;
    }

    impl_->context.contextCreationStarted = true;
    impl_->context.SetReadyCallback(std::move(callback));

    if (!impl_->context.CreateInstance())
    {
        impl_->context.FinishInitialization(false, "Could not create WebGPU instance.");
        return;
    }

    LOG_INFO << "WebGPU instance created.";

    if (!impl_->context.CreateCanvasSurface())
    {
        impl_->context.FinishInitialization(false, "Could not create WebGPU surface for canvas.");
        return;
    }

    LOG_INFO << "WebGPU surface created.";

    if (!impl_->context.RequestAdapter())
    {
        impl_->context.FinishInitialization(false, "Could not request WebGPU adapter.");
        return;
    }

    LOG_INFO << "WebGPU adapter request started.";
}
void WebGpuApi::ProcessEvents()
{
    impl_->context.ProcessEvents();
}

IWindowApi& WebGpuApi::GetWindowApi()
{
    return *impl_->windowApi;
}
void WebGpuApi::Init()
{
    LOG_DEBUG << "WebGpuApi::Init";

    if (!impl_->context.IsReady())
    {
        throw std::runtime_error("WebGPU device is not ready");
    }

    impl_->shaderManager.SetDevice(impl_->context.device, impl_->context.surfaceFormat);
    impl_->textureManager.SetDevice(impl_->context.device, impl_->context.queue);

    if (!impl_->triangleRenderer.IsReady())
    {
        if (!impl_->triangleRenderer.Initialize(impl_->context.device, impl_->context.surfaceFormat, impl_->context.queue))
        {
            throw std::runtime_error("WebGPU triangle renderer initialization failed");
        }
    }
}
void WebGpuApi::PrepareFrame()
{
    ProcessEvents();

    if (!impl_->triangleRenderer.IsReady() && impl_->context.IsReady())
    {
        impl_->triangleRenderer.Initialize(impl_->context.device, impl_->context.surfaceFormat, impl_->context.queue);
    }

    auto& context = impl_->context;

    if (!context.IsReady() || !context.IsConfigured() || !context.surface || !context.queue)
    {
        return;
    }

    if (context.frameInProgress)
    {
        LOG_ERROR << "Previous WebGPU frame was not finished.";
        return;
    }

    WGPUSurfaceTexture surfaceTexture{};
    wgpuSurfaceGetCurrentTexture(context.surface, &surfaceTexture);

    if (!surfaceTexture.texture)
    {
        LOG_ERROR << "Could not acquire surface texture. Status: " << static_cast<uint32>(surfaceTexture.status);
        return;
    }

    context.frameTexture = surfaceTexture.texture;

    if (surfaceTexture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessOptimal &&
        surfaceTexture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessSuboptimal)
    {
        LOG_ERROR << "Surface texture acquisition failed. Status: " << static_cast<uint32>(surfaceTexture.status);
        context.ReleaseFrameResources();
        return;
    }

    context.frameTextureView = wgpuTextureCreateView(context.frameTexture, nullptr);
    if (!context.frameTextureView)
    {
        LOG_ERROR << "Could not create surface texture view.";
        context.ReleaseFrameResources();
        return;
    }

    WGPUCommandEncoderDescriptor encoderDescriptor{};
    encoderDescriptor.label = WGPUStringView{"WebGPU Frame Encoder", WGPU_STRLEN};

    context.frameCommandEncoder = wgpuDeviceCreateCommandEncoder(context.device, &encoderDescriptor);
    if (!context.frameCommandEncoder)
    {
        LOG_ERROR << "Could not create command encoder.";
        context.ReleaseFrameResources();
        return;
    }

    WGPURenderPassColorAttachment colorAttachment{};
    colorAttachment.view = context.frameTextureView;
    colorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
    colorAttachment.loadOp = WGPULoadOp_Clear;
    colorAttachment.storeOp = WGPUStoreOp_Store;
    colorAttachment.clearValue = WGPUColor{bgColor.value.x, bgColor.value.y, bgColor.value.z, bgColor.value.w};

    WGPURenderPassDescriptor renderPassDescriptor{};
    renderPassDescriptor.label = WGPUStringView{"WebGPU Frame Render Pass", WGPU_STRLEN};
    renderPassDescriptor.colorAttachmentCount = 1;
    renderPassDescriptor.colorAttachments = &colorAttachment;

    context.frameRenderPass = wgpuCommandEncoderBeginRenderPass(context.frameCommandEncoder, &renderPassDescriptor);
    if (!context.frameRenderPass)
    {
        LOG_ERROR << "Could not create render pass.";
        context.ReleaseFrameResources();
        return;
    }

    context.frameInProgress = true;

    if (!impl_->queuedDrawCalls.empty())
    {
        for (const auto& drawCall : impl_->queuedDrawCalls)
        {
            DrawQueuedMesh(impl_->shaderManager, impl_->meshManager, context.device, context.frameRenderPass, drawCall,
                           impl_->shaderBuffers, impl_->textureManager);
        }

        impl_->queuedDrawCalls.clear();
    }
    else if (impl_->triangleRenderer.IsReady())
    {
        impl_->triangleRenderer.Draw(context.frameRenderPass);
    }
}
void WebGpuApi::EndFrame()
{
    auto& context = impl_->context;

    if (!context.frameInProgress)
        return;

    wgpuRenderPassEncoderEnd(context.frameRenderPass);
    wgpuRenderPassEncoderRelease(context.frameRenderPass);
    context.frameRenderPass = nullptr;

    WGPUCommandBuffer commandBuffer = wgpuCommandEncoderFinish(context.frameCommandEncoder, nullptr);
    wgpuCommandEncoderRelease(context.frameCommandEncoder);
    context.frameCommandEncoder = nullptr;

    if (!commandBuffer)
    {
        LOG_ERROR << "Could not finish command encoder.";
        context.ReleaseFrameResources();
        return;
    }

    wgpuQueueSubmit(context.queue, 1, &commandBuffer);
    wgpuCommandBufferRelease(commandBuffer);

    if (context.surface)
    {
        wgpuSurfacePresent(context.surface);
    }

    context.ReleaseFrameResources();
}
void WebGpuApi::SetShadersFilesLocations(const std::filesystem::path& path)
{
    impl_->shaderManager.SetShadersFilesLocations(path);
}
void WebGpuApi::DebugNormalMeshGeneration(bool)
{
}
void WebGpuApi::SetShaderQuaility(ShaderQuaility quality)
{
    impl_->shaderManager.SetShaderQuaility(quality);
}
void WebGpuApi::CreateContext()
{
    if (!impl_->context.IsReady())
    {
        throw std::runtime_error("Cannot create graphics context: WebGPU device is not ready.");
    }
}
void WebGpuApi::DeleteContext()
{
    for (auto& [id, buffer] : impl_->shaderBuffers)
    {
        if (buffer.buffer)
        {
            wgpuBufferDestroy(buffer.buffer);
            wgpuBufferRelease(buffer.buffer);
            buffer.buffer = nullptr;
        }
        (void)id;
    }
    impl_->shaderBuffers.clear();
    impl_->boundShaderBuffers.clear();

    impl_->context.Release();
    impl_->triangleRenderer.Release();
    impl_->shaderManager.Release();

    impl_->context.contextCreationStarted = false;
    impl_->context.contextReadyCallback   = {};
}
void WebGpuApi::PrintVersion()
{
}
bool WebGpuApi::IsTesselationSupported() const
{
    return {};
}
std::vector<RendererType> WebGpuApi::GetSupportedRenderers() const
{
    return {RendererType::SIMPLE};
}
void WebGpuApi::SetDefaultTarget()
{
}
void WebGpuApi::SetBackgroundColor(const Color& color)
{
    bgColor = color;
}
const Color& WebGpuApi::GetBackgroundColor() const
{
    return bgColor;
}
void WebGpuApi::EnableDepthTest()
{
}
void WebGpuApi::DisableDepthTest()
{
}
ID WebGpuApi::CreateShader(ShaderProgramType shaderType)
{
    return impl_->shaderManager.Create(shaderType);
}
ID WebGpuApi::CreateShaderBuffer(uint32 bindLocation, uint32 size, DrawFlag)
{
    if (!impl_->context.IsReady() || !impl_->context.device)
    {
        return std::nullopt;
    }

    WGPUBufferDescriptor descriptor{};
    descriptor.label = WGPUStringView{"WebGPU Uniform Buffer", WGPU_STRLEN};
    descriptor.size = size;
    descriptor.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;
    descriptor.mappedAtCreation = WGPU_FALSE;

    WGPUBuffer buffer = wgpuDeviceCreateBuffer(impl_->context.device, &descriptor);
    if (!buffer)
    {
        return std::nullopt;
    }

    const auto id = impl_->nextShaderBufferId++;
    impl_->shaderBuffers[id] = WebGpuShaderBufferResource{buffer, bindLocation, size, false, std::vector<uint8_t>(size)};
    return id;
}
ID WebGpuApi::CreateShaderStorageBuffer(uint32 bindLocation, uint32 size, DrawFlag)
{
    if (!impl_->context.IsReady() || !impl_->context.device)
    {
        return std::nullopt;
    }

    WGPUBufferDescriptor descriptor{};
    descriptor.label = WGPUStringView{"WebGPU Storage Buffer", WGPU_STRLEN};
    descriptor.size = size;
    descriptor.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Storage;
    descriptor.mappedAtCreation = WGPU_FALSE;

    WGPUBuffer buffer = wgpuDeviceCreateBuffer(impl_->context.device, &descriptor);
    if (!buffer)
    {
        return std::nullopt;
    }

    const auto id = impl_->nextShaderBufferId++;
    impl_->shaderBuffers[id] = WebGpuShaderBufferResource{buffer, bindLocation, size, true, std::vector<uint8_t>(size)};
    return id;
}
void WebGpuApi::UpdateShaderBuffer(uint32 id, void const* buffer)
{
    const auto it = impl_->shaderBuffers.find(id);
    if (it == impl_->shaderBuffers.end() || buffer == nullptr)
    {
        return;
    }

    auto& shaderBuffer = it->second;
    std::memcpy(shaderBuffer.hostData.data(), buffer, shaderBuffer.size);
    if (shaderBuffer.buffer && impl_->context.queue)
    {
        wgpuQueueWriteBuffer(impl_->context.queue, shaderBuffer.buffer, 0, shaderBuffer.hostData.data(), shaderBuffer.size);
    }
}
void WebGpuApi::UpdateShaderStorageBuffer(uint32 id, void const* buffer, uint32 newSize)
{
    const auto it = impl_->shaderBuffers.find(id);
    if (it == impl_->shaderBuffers.end() || buffer == nullptr)
    {
        return;
    }

    auto& shaderBuffer = it->second;
    if (newSize > shaderBuffer.size)
    {
        shaderBuffer.size = newSize;
        shaderBuffer.hostData.resize(newSize);
    }

    std::memcpy(shaderBuffer.hostData.data(), buffer, newSize);
    if (shaderBuffer.buffer && impl_->context.queue)
    {
        wgpuQueueWriteBuffer(impl_->context.queue, shaderBuffer.buffer, 0, shaderBuffer.hostData.data(), newSize);
    }
}
void* WebGpuApi::MapShaderStorageBuffer(uint32 id, uint32 bufferSize, uint32 flags)
{
    const auto it = impl_->shaderBuffers.find(id);
    if (it == impl_->shaderBuffers.end())
    {
        return nullptr;
    }

    auto& shaderBuffer = it->second;
    if (bufferSize > shaderBuffer.hostData.size())
    {
        shaderBuffer.hostData.resize(bufferSize);
    }

    (void)flags;
    return shaderBuffer.hostData.data();
}
void WebGpuApi::UnmapShaderStorageBuffer(uint32)
{
}
uint32 WebGpuApi::BindShaderBuffer(uint32 id)
{
    const auto it = impl_->shaderBuffers.find(id);
    if (it == impl_->shaderBuffers.end())
    {
        return std::numeric_limits<uint32>::max();
    }

    const auto bindLocation = it->second.bindLocation;
    const auto previous = impl_->boundShaderBuffers.contains(bindLocation) ? impl_->boundShaderBuffers[bindLocation] : 0u;
    impl_->boundShaderBuffers[bindLocation] = id;

    RebuildBindGroupForCurrentProgram(impl_->shaderManager, impl_->context.device, impl_->shaderBuffers,
                                     impl_->boundShaderBuffers, impl_->textureManager);
    return previous;
}
void WebGpuApi::UseShader(uint32 shaderId)
{
    impl_->shaderManager.UseShader(shaderId);
    RebuildBindGroupForCurrentProgram(impl_->shaderManager, impl_->context.device, impl_->shaderBuffers,
                                     impl_->boundShaderBuffers, impl_->textureManager);
}
ID WebGpuApi::CreateTexture(const Utils::Image& image, TextureFilter filter, TextureMipmap mipmap)
{
    return impl_->textureManager.CreateTexture(image, filter, mipmap);
}
ID WebGpuApi::CreateTexture(const std::vector<Utils::Image>& images, GraphicsApi::TextureFilter filter,
                           GraphicsApi::TextureMipmap mipmap)
{
    return impl_->textureManager.CreateTexture(images, filter, mipmap);
}
ID WebGpuApi::CreateTextureStorage(TextureType type, TextureFilter filter, int32 samples)
{
    return impl_->textureManager.CreateTextureStorage(type, filter, samples);
}
ID WebGpuApi::CreateCubMapTexture(const CubemapImages& cubemapImages)
{
    return impl_->textureManager.CreateCubMapTexture(cubemapImages);
}
std::optional<uint64_t> WebGpuApi::GetBindlessHandle(IdType id)
{
    return impl_->textureManager.GetBindlessHandle(id);
}
void WebGpuApi::GenerateMipmaps(IdType id)
{
    impl_->textureManager.GenerateMipmaps(id);
}
void WebGpuApi::CopyTexture(IdType sourceId, IdType targetId, uint32 xOffset, uint32 yOffset)
{
    impl_->textureManager.CopyTexture(sourceId, targetId, xOffset, yOffset);
}
void WebGpuApi::UpdateTexture(uint32 id, const vec2ui& offset, const Utils::Image& image)
{
    impl_->textureManager.UpdateTexture(id, offset, image);
}
void WebGpuApi::UpdateTexture(uint32 id, const Utils::Image& image)
{
    impl_->textureManager.UpdateTexture(id, image);
}
void WebGpuApi::ClearTexture(uint32 id, const Color& color)
{
    impl_->textureManager.ClearTexture(id, color);
}
void WebGpuApi::EnableBlend()
{
}
void WebGpuApi::DisableBlend()
{
}
void WebGpuApi::EnableDepthMask()
{
}
void WebGpuApi::DisableDepthMask()
{
}
void WebGpuApi::ActiveTexture(uint32 unit, uint32 textureId)
{
    impl_->textureManager.ActiveTexture(unit, textureId);
}
void WebGpuApi::ActiveTexture(uint32 unit)
{
    impl_->textureManager.ActiveTexture(unit);
}

void WebGpuApi::DeleteObject(uint32 id)
{
    if (impl_->shaderBuffers.contains(id))
    {
        DeleteShaderBuffer(id);
        return;
    }

    impl_->textureManager.DeleteTexture(id);
    RebuildBindGroupForCurrentProgram(impl_->shaderManager, impl_->context.device, impl_->shaderBuffers,
                                     impl_->boundShaderBuffers, impl_->textureManager);
}
void WebGpuApi::DeleteObject(const std::vector<uint32>& ids)
{
    for (const auto id : ids)
    {
        DeleteObject(id);
    }
}

void WebGpuApi::DeleteShaderBuffer(uint32 id)
{
    const auto it = impl_->shaderBuffers.find(id);
    if (it == impl_->shaderBuffers.end())
    {
        return;
    }

    if (it->second.buffer)
    {
        wgpuBufferDestroy(it->second.buffer);
        wgpuBufferRelease(it->second.buffer);
    }

    for (auto itBinding = impl_->boundShaderBuffers.begin(); itBinding != impl_->boundShaderBuffers.end();)
    {
        if (itBinding->second == id)
        {
            itBinding = impl_->boundShaderBuffers.erase(itBinding);
        }
        else
        {
            ++itBinding;
        }
    }

    impl_->shaderBuffers.erase(it);
    RebuildBindGroupForCurrentProgram(impl_->shaderManager, impl_->context.device, impl_->shaderBuffers,
                                     impl_->boundShaderBuffers, impl_->textureManager);
}
void WebGpuApi::DeleteShaderBuffer(const std::vector<uint32>& ids)
{
    for (const auto id : ids)
    {
        DeleteShaderBuffer(id);
    }
}
std::string WebGpuApi::GetBufferStatus()
{
    return {};
}
ID WebGpuApi::CreatePatchMesh(const std::vector<float>&)
{
    return {};
}
ID WebGpuApi::CreatePurePatchMeshInstanced(uint32, uint32)
{
    return {};
}
ID WebGpuApi::CreateMesh(const MeshRawData& meshData, RenderType renderType)
{
    impl_->meshManager.SetDevice(impl_->context.device, impl_->context.queue);
    return impl_->meshManager.CreateMesh(meshData, renderType);
}

ID WebGpuApi::CreateDynamicLineMesh()
{
    return {};
}
ID WebGpuApi::CreateParticle()
{
    return {};
}
ID WebGpuApi::CreateAnimatedParticle()
{
    return {};
}

void WebGpuApi::Compute(uint32, uint32, uint32)
{
}

void WebGpuApi::RenderPurePatchedMeshInstances(uint32)
{
}
void WebGpuApi::RenderMesh(uint32 meshId)
{
    const auto activeProgramId = impl_->shaderManager.GetCurrentProgramId();
    if (meshId == 0 || activeProgramId == 0)
    {
        return;
    }

    const auto* mesh = impl_->meshManager.GetMeshResource(meshId);
    if (!mesh)
    {
        return;
    }

    WebGpuDrawCall drawCall{};
    drawCall.programId = activeProgramId;
    drawCall.meshId = meshId;
    drawCall.boundShaderBuffers = impl_->boundShaderBuffers;
    drawCall.activeTextureAssignments = impl_->textureManager.GetActiveTextureAssignments();
    impl_->queuedDrawCalls.push_back(drawCall);
}
void WebGpuApi::RenderProcedural(uint32 count)
{
    if (count == 0)
    {
        return;
    }

    const auto activeProgramId = impl_->shaderManager.GetCurrentProgramId();
    if (activeProgramId == 0)
    {
        return;
    }

    WebGpuDrawCall drawCall{};
    drawCall.programId = activeProgramId;
    drawCall.proceduralVertexCount = count;
    drawCall.isProcedural = true;
    drawCall.boundShaderBuffers = impl_->boundShaderBuffers;
    drawCall.activeTextureAssignments = impl_->textureManager.GetActiveTextureAssignments();
    impl_->queuedDrawCalls.push_back(drawCall);
}
void WebGpuApi::RenderDebugNormals(uint32)
{
}
void WebGpuApi::RenderTriangleStripMesh(uint32)
{
}
void WebGpuApi::RenderMeshInstanced(uint32, uint32)
{
}
void WebGpuApi::RenderPoints(uint32)
{
}
void WebGpuApi::RenderQuad()
{
    if (!impl_->context.IsReady() || !impl_->context.device)
    {
        return;
    }

    if (!impl_->quadMeshId.has_value())
    {
        MeshRawData quadMeshData{};
        quadMeshData.positions_ = {
            -1.0f, -1.0f, 0.0f,
             1.0f, -1.0f, 0.0f,
             1.0f,  1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,
        };
        quadMeshData.textCoords_ = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
        };
        quadMeshData.indices_ = {0u, 1u, 2u, 0u, 2u, 3u};
        quadMeshData.normals_ = {
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
        };
        quadMeshData.tangents_ = {
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
        };

        impl_->meshManager.SetDevice(impl_->context.device, impl_->context.queue);
        impl_->quadMeshId = impl_->meshManager.CreateMesh(quadMeshData, RenderType::TRIANGLES);
    }

    if (impl_->quadMeshId.has_value())
    {
        RenderMesh(*impl_->quadMeshId);
    }
}
void WebGpuApi::RenderQuadTs()
{
    RenderQuad();
}
void WebGpuApi::EnableClipingPlane(uint32)
{
}
void WebGpuApi::DisableCliping(uint32)
{
}
void WebGpuApi::EnableCulling()
{
}
void WebGpuApi::DisableCulling()
{
}
void WebGpuApi::SetViewPort(uint32, uint32, uint32, uint32)
{
}
void WebGpuApi::BindTexture(uint32 textureId) const
{
    impl_->textureManager.BindTexture(textureId);
}
void WebGpuApi::BindImageTexture(uint32 textureId, TextureAccess access)
{
    impl_->textureManager.BindImageTexture(textureId, access);
}
ID WebGpuApi::CreateShadowMap(uint32, uint32)
{
    return {};
}
void WebGpuApi::PolygonModeRender()
{
}
void WebGpuApi::LineModeRender()
{
}
void WebGpuApi::SetBlendFunction(BlendFunctionType)
{
}
void WebGpuApi::UpdateMatrixes(uint32, const std::vector<mat4>&)
{
}
void WebGpuApi::UpdateMesh(uint32 id, const MeshRawData& meshData, const std::set<VertexBufferObjects>& dirtyObjects)
{
    impl_->meshManager.UpdateMesh(id, meshData, dirtyObjects);
}
void WebGpuApi::UpdateLineMesh(uint32, const LineMesh&)
{
}
void WebGpuApi::UpdateOffset(uint32, const std::vector<vec4>&)
{
}
void WebGpuApi::UpdateBlend(uint32, const std::vector<float>&)
{
}
void WebGpuApi::LoadViewMatrix(const mat4& viewMatrix)
{
    impl_->viewMatrix = viewMatrix;
}
void WebGpuApi::LoadProjectionMatrix(const mat4& projectionMatrix)
{
    impl_->projectionMatrix = projectionMatrix;
}
void WebGpuApi::DrawLine(const vec3&, const vec3&, const vec3&)
{
}
mat4 WebGpuApi::PrepareMatrixToLoad(const mat4& matrix)
{
    return matrix;
}
std::vector<uint8> WebGpuApi::GetTextureData(uint32 textureId) const
{
    return impl_->textureManager.GetTextureData(textureId);
}
const TextureInfo& WebGpuApi::GetTextureInfo(uint32 textureId) const
{
    return impl_->textureManager.GetTextureInfo(textureId);
}
void WebGpuApi::TakeSnapshot(const std::filesystem::path&) const
{
}
std::optional<Utils::Image> WebGpuApi::GetImage(IdType id) const
{
    return impl_->textureManager.GetImage(id);
}
std::vector<Utils::Image> WebGpuApi::GetImageArray(IdType id) const
{
    return impl_->textureManager.GetImageArray(id);
}
IFrameBuffer& WebGpuApi::GetDefaultFrameBuffer()
{
    return impl_->frameBufferManager.GetDefaultFrameBuffer();
}
IFrameBuffer& WebGpuApi::CreateFrameBuffer(const std::vector<FrameBuffer::Attachment>& attachments)
{
    return impl_->frameBufferManager.CreateFrameBuffer(attachments);
}
void WebGpuApi::DeleteFrameBuffer(IFrameBuffer& framebuffer)
{
    impl_->frameBufferManager.DeleteFrameBuffer(framebuffer);
}
}  // namespace GraphicsApi::WebGpu
