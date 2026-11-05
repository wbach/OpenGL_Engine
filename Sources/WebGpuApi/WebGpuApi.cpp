#include "WebGpuApi.h"

#include <Logger/Log.h>
#include <emscripten/html5.h>
#include <webgpu/webgpu.h>

#include <stdexcept>
#include <utility>

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
struct WebGpuApi::Impl
{
    std::unique_ptr<IWindowApi> windowApi;
    WebGpuContext context;
    WebGpuTriangleRenderer triangleRenderer;
    WebGpuTextureManager textureManager;
    WebGpuMeshManager meshManager;
    WebGpuShaderManager shaderManager;
    WebGpuFrameBufferManager frameBufferManager;

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

    if (impl_->triangleRenderer.IsReady())
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
    impl_->context.Release();
    impl_->triangleRenderer.Release();

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
ID WebGpuApi::CreateShaderBuffer(uint32, uint32, DrawFlag)
{
    return {};
}
ID WebGpuApi::CreateShaderStorageBuffer(uint32, uint32, DrawFlag)
{
    return {};
}
void WebGpuApi::UpdateShaderBuffer(uint32, void const*)
{
}
void WebGpuApi::UpdateShaderStorageBuffer(uint32, void const*, uint32)
{
}
void* WebGpuApi::MapShaderStorageBuffer(uint32, uint32, uint32)
{
    return nullptr;
}
void WebGpuApi::UnmapShaderStorageBuffer(uint32)
{
}
uint32 WebGpuApi::BindShaderBuffer(uint32)
{
    return {};
}
void WebGpuApi::UseShader(uint32 shaderId)
{
    impl_->shaderManager.UseShader(shaderId);
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

void WebGpuApi::DeleteObject(uint32)
{
}
void WebGpuApi::DeleteObject(const std::vector<uint32>&)
{
}

void WebGpuApi::DeleteShaderBuffer(uint32)
{
}
void WebGpuApi::DeleteShaderBuffer(const std::vector<uint32>&)
{
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
void WebGpuApi::RenderMesh(uint32)
{
}
void WebGpuApi::RenderProcedural(uint32)
{
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
}
void WebGpuApi::RenderQuadTs()
{
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
void WebGpuApi::LoadViewMatrix(const mat4&)
{
}
void WebGpuApi::LoadProjectionMatrix(const mat4&)
{
}
void WebGpuApi::DrawLine(const vec3&, const vec3&, const vec3&)
{
}
mat4 WebGpuApi::PrepareMatrixToLoad(const mat4&)
{
    return {};
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
