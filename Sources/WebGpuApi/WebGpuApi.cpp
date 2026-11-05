#include "WebGpuApi.h"

#include <Logger/Log.h>
#include <emscripten/html5.h>
#include <webgpu/webgpu.h>

#include <stdexcept>
#include <utility>

#include "DefaultFrameBuffer.h"
#include "WebGpuContext.h"
#include "WebGpuTriangleRenderer.h"
#include "WebGpuWindowApi.h"

namespace GraphicsApi::WebGpu
{
struct WebGpuApi::Impl
{
    std::unique_ptr<IWindowApi> windowApi;
    WebGpuContext context;
    WebGpuTriangleRenderer triangleRenderer;
    DefaultFrameBuffer defaultFrameBuffer;

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
void WebGpuApi::SetShadersFilesLocations(const std::filesystem::path&)
{
}
void WebGpuApi::DebugNormalMeshGeneration(bool)
{
}
void WebGpuApi::SetShaderQuaility(ShaderQuaility)
{
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
ID WebGpuApi::CreateShader(ShaderProgramType)
{
    return {};
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
void WebGpuApi::UseShader(uint32)
{
}
ID WebGpuApi::CreateTexture(const Utils::Image&, TextureFilter, TextureMipmap)
{
    return {};
}
ID WebGpuApi::CreateTexture(const std::vector<Utils::Image>&, GraphicsApi::TextureFilter, GraphicsApi::TextureMipmap)
{
    return {};
}
ID WebGpuApi::CreateTextureStorage(TextureType, TextureFilter, int32)
{
    return {};
}
ID WebGpuApi::CreateCubMapTexture(const CubemapImages&)
{
    return {};
}
std::optional<uint64_t> WebGpuApi::GetBindlessHandle(IdType)
{
    return {};
}
void WebGpuApi::GenerateMipmaps(IdType)
{
}
void WebGpuApi::CopyTexture(IdType, IdType, uint32, uint32)
{
}
void WebGpuApi::UpdateTexture(uint32, const vec2ui&, const Utils::Image&)
{
}
void WebGpuApi::UpdateTexture(uint32, const Utils::Image&)
{
}
void WebGpuApi::ClearTexture(uint32, const Color&)
{
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
void WebGpuApi::ActiveTexture(uint32, uint32)
{
}
void WebGpuApi::ActiveTexture(uint32)
{
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
ID WebGpuApi::CreateMesh(const MeshRawData&, RenderType)
{
    return {};
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
void WebGpuApi::BindTexture(uint32) const
{
}
void WebGpuApi::BindImageTexture(uint32, TextureAccess)
{
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
void WebGpuApi::UpdateMesh(uint32, const MeshRawData&, const std::set<VertexBufferObjects>&)
{
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
std::vector<uint8> WebGpuApi::GetTextureData(uint32) const
{
    return {};
}
const TextureInfo& WebGpuApi::GetTextureInfo(uint32) const
{
    static TextureInfo info;
    return info;
}
void WebGpuApi::TakeSnapshot(const std::filesystem::path&) const
{
}
std::optional<Utils::Image> WebGpuApi::GetImage(IdType) const
{
    return {};
}
std::vector<Utils::Image> WebGpuApi::GetImageArray(IdType) const
{
    return {};
}
IFrameBuffer& WebGpuApi::GetDefaultFrameBuffer()
{
    return impl_->defaultFrameBuffer;
}
IFrameBuffer& WebGpuApi::CreateFrameBuffer(const std::vector<FrameBuffer::Attachment>&)
{
    return impl_->defaultFrameBuffer;
}
void WebGpuApi::DeleteFrameBuffer(IFrameBuffer&)
{
}
}  // namespace GraphicsApi::WebGpu
