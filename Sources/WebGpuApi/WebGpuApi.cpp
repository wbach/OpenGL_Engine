#include "WebGpuApi.h"

#include <Logger/Log.h>
#include <emscripten/html5.h>
#include <webgpu/webgpu.h>

#include <stdexcept>
#include <utility>

#include "DefaultFrameBuffer.h"
#include "WebGpuWindowApi.h"

namespace GraphicsApi::WebGpu
{
struct WebGpuApi::Impl
{
    std::unique_ptr<IWindowApi> windowApi;

    WGPUInstance instance            = nullptr;
    WGPUAdapter adapter              = nullptr;
    WGPUDevice device                = nullptr;
    WGPUQueue queue                  = nullptr;
    WGPUSurface surface              = nullptr;
    WGPUTextureFormat surfaceFormat  = WGPUTextureFormat_Undefined;
    WGPUTexture frameTexture         = nullptr;
    WGPUTextureView frameTextureView = nullptr;

    WGPUCommandEncoder frameCommandEncoder = nullptr;
    WGPURenderPassEncoder frameRenderPass  = nullptr;

    WGPUShaderModule triangleShaderModule = nullptr;
    WGPURenderPipeline trianglePipeline   = nullptr;

    bool frameInProgress = false;

    uint32 surfaceWidth  = 0;
    uint32 surfaceHeight = 0;

    bool surfaceConfigured = false;

    bool deviceReady            = false;
    bool contextCreationStarted = false;

    ContextReadyCallback contextReadyCallback;

    DefaultFrameBuffer defaultFrameBuffer;

    Impl()
        : windowApi{std::make_unique<WebGpuWindowApi>()}
    {
    }

    bool ConfigureSurface()
    {
        constexpr int CANVAS_WIDTH  = 1280;
        constexpr int CANVAS_HEIGHT = 720;

        const auto resizeResult = emscripten_set_canvas_element_size("canvas", CANVAS_WIDTH, CANVAS_HEIGHT);

        if (resizeResult != EMSCRIPTEN_RESULT_SUCCESS)
        {
            LOG_WARN << "Could not resize HTML canvas.";
        }

        int width  = 0;
        int height = 0;

        const auto result = emscripten_get_canvas_element_size("canvas", &width, &height);

        if (result != EMSCRIPTEN_RESULT_SUCCESS || width <= 0 || height <= 0)
        {
            LOG_ERROR << "Could not get canvas size.";
            return false;
        }

        // ---------------------------------------------
        // Pobranie możliwości Surface
        // ---------------------------------------------

        WGPUSurfaceCapabilities capabilities{};

        const WGPUStatus status = wgpuSurfaceGetCapabilities(surface, adapter, &capabilities);

        if (status != WGPUStatus_Success)
        {
            wgpuSurfaceCapabilitiesFreeMembers(capabilities);

            LOG_ERROR << "Could not get surface capabilities.";

            return false;
        }

        if (capabilities.formatCount == 0 || capabilities.formats == nullptr)
        {
            wgpuSurfaceCapabilitiesFreeMembers(capabilities);

            LOG_ERROR << "No supported surface formats.";

            return false;
        }

        // ---------------------------------------------
        // Konfiguracja Surface
        // ---------------------------------------------

        WGPUSurfaceConfiguration config{};

        config.device = device;

        // Pierwszy format jest preferowany przez adapter.
        config.format = capabilities.formats[0];

        config.usage = WGPUTextureUsage_RenderAttachment;

        config.width  = static_cast<uint32>(width);
        config.height = static_cast<uint32>(height);

        config.presentMode = WGPUPresentMode_Fifo;

        config.alphaMode = WGPUCompositeAlphaMode_Auto;

        // ---------------------------------------------
        // Zastosowanie konfiguracji
        // ---------------------------------------------

        wgpuSurfaceConfigure(surface, &config);

        wgpuSurfaceCapabilitiesFreeMembers(capabilities);

        // ---------------------------------------------
        // Zapisanie konfiguracji
        // ---------------------------------------------

        surfaceFormat = config.format;

        surfaceWidth  = config.width;
        surfaceHeight = config.height;

        surfaceConfigured = true;

        LOG_INFO << "WebGPU surface configured: " << surfaceWidth << "x" << surfaceHeight;

        return true;
    }

    void Release()
    {
        if (trianglePipeline)
        {
            wgpuRenderPipelineRelease(trianglePipeline);
            trianglePipeline = nullptr;
        }

        if (triangleShaderModule)
        {
            wgpuShaderModuleRelease(triangleShaderModule);
            triangleShaderModule = nullptr;
        }

        ReleaseFrameResources();

        deviceReady = false;

        if (surface && surfaceConfigured)
        {
            wgpuSurfaceUnconfigure(surface);
            surfaceConfigured = false;
        }

        if (queue)
        {
            wgpuQueueRelease(queue);
            queue = nullptr;
        }

        if (device)
        {
            wgpuDeviceRelease(device);
            device = nullptr;
        }

        if (surface)
        {
            wgpuSurfaceRelease(surface);
            surface = nullptr;
        }

        if (adapter)
        {
            wgpuAdapterRelease(adapter);
            adapter = nullptr;
        }

        if (instance)
        {
            wgpuInstanceRelease(instance);
            instance = nullptr;
        }
    }

    void ReleaseFrameResources()
    {
        if (frameRenderPass)
        {
            wgpuRenderPassEncoderEnd(frameRenderPass);
            wgpuRenderPassEncoderRelease(frameRenderPass);

            frameRenderPass = nullptr;
        }

        if (frameCommandEncoder)
        {
            wgpuCommandEncoderRelease(frameCommandEncoder);
            frameCommandEncoder = nullptr;
        }

        if (frameTextureView)
        {
            wgpuTextureViewRelease(frameTextureView);
            frameTextureView = nullptr;
        }

        if (frameTexture)
        {
            wgpuTextureRelease(frameTexture);
            frameTexture = nullptr;
        }

        frameInProgress = false;
    }

    static std::string GetMessage(WGPUStringView message)
    {
        if (!message.data)
            return {};

        if (message.length == WGPU_STRLEN)
            return message.data;

        return std::string(message.data, message.length);
    }

    void FinishInitialization(bool success, const std::string& message = {})
    {
        deviceReady = success;

        if (!success)
        {
            LOG_ERROR << "WebGPU initialization failed: " << message;
        }
        else
        {
            LOG_INFO << "WebGPU device initialized successfully.";
        }

        auto callback = std::move(contextReadyCallback);

        if (callback)
        {
            callback(success);
        }
    }

    static void OnAdapterRequested(WGPURequestAdapterStatus status, WGPUAdapter requestedAdapter, WGPUStringView message,
                                   void* userdata1, void*)
    {
        auto* self = static_cast<Impl*>(userdata1);

        if (!self)
            return;

        if (status != WGPURequestAdapterStatus_Success || !requestedAdapter)
        {
            self->FinishInitialization(false, GetMessage(message));

            return;
        }

        self->adapter = requestedAdapter;

        LOG_INFO << "WebGPU adapter acquired.";

        // Request device.
        WGPUDeviceDescriptor deviceDescriptor{};

        WGPURequestDeviceCallbackInfo callbackInfo{};
        callbackInfo.mode      = WGPUCallbackMode_AllowProcessEvents;
        callbackInfo.callback  = &Impl::OnDeviceRequested;
        callbackInfo.userdata1 = self;

        wgpuAdapterRequestDevice(self->adapter, &deviceDescriptor, callbackInfo);
    }

    static void OnDeviceRequested(WGPURequestDeviceStatus status, WGPUDevice requestedDevice, WGPUStringView message,
                                  void* userdata1, void*)
    {
        auto* self = static_cast<Impl*>(userdata1);

        if (!self)
            return;

        if (status != WGPURequestDeviceStatus_Success || !requestedDevice)
        {
            self->FinishInitialization(false, GetMessage(message));

            return;
        }

        self->device = requestedDevice;

        LOG_INFO << "WebGPU device acquired.";

        // Get GPU queue.
        self->queue = wgpuDeviceGetQueue(self->device);

        if (!self->queue)
        {
            self->FinishInitialization(false, "Could not acquire WebGPU queue.");

            return;
        }

        // Configure rendering surface.
        if (!self->ConfigureSurface())
        {
            self->FinishInitialization(false, "Could not configure WebGPU surface.");

            return;
        }

        // Create initial triangle pipeline.
        if (!self->CreateTrianglePipeline())
        {
            self->FinishInitialization(false, "Could not create triangle pipeline.");

            return;
        }

        self->FinishInitialization(true);
    }

    bool CreateTrianglePipeline()
    {
        // ---------------------------------------------
        // 1. WGSL shader source
        // ---------------------------------------------

        static constexpr char shaderCode[] = R"WGSL(

        @vertex
        fn vs_main(
            @builtin(vertex_index) vertexIndex: u32
        ) -> @builtin(position) vec4<f32>
        {
            var positions = array<vec2<f32>, 3>(
                vec2<f32>( 0.0,  0.7),
                vec2<f32>(-0.7, -0.65),
                vec2<f32>( 0.7, -0.65)
            );

            return vec4<f32>(
                positions[vertexIndex],
                0.0,
                1.0
            );
        }


        @fragment
        fn fs_main() -> @location(0) vec4<f32>
        {
            return vec4<f32>(
                0.1,
                0.9,
                0.25,
                1.0
            );
        }

    )WGSL";

        // ---------------------------------------------
        // 2. Create shader module
        // ---------------------------------------------

        WGPUShaderSourceWGSL wgslSource{};

        wgslSource.chain.sType = WGPUSType_ShaderSourceWGSL;
        wgslSource.chain.next  = nullptr;

        wgslSource.code = WGPUStringView{shaderCode, WGPU_STRLEN};

        WGPUShaderModuleDescriptor shaderDescriptor{};

        shaderDescriptor.nextInChain = &wgslSource.chain;

        shaderDescriptor.label = WGPUStringView{"Triangle Shader", WGPU_STRLEN};

        triangleShaderModule = wgpuDeviceCreateShaderModule(device, &shaderDescriptor);

        if (!triangleShaderModule)
        {
            LOG_ERROR << "Could not create triangle shader module.";
            return false;
        }

        // ---------------------------------------------
        // 3. Configure color target
        // ---------------------------------------------

        WGPUColorTargetState colorTarget{};

        colorTarget.format = surfaceFormat;

        colorTarget.blend = nullptr;

        colorTarget.writeMask = WGPUColorWriteMask_All;

        // ---------------------------------------------
        // 4. Configure fragment stage
        // ---------------------------------------------

        WGPUFragmentState fragmentState{};

        fragmentState.module = triangleShaderModule;

        fragmentState.entryPoint = WGPUStringView{"fs_main", WGPU_STRLEN};

        fragmentState.targetCount = 1;

        fragmentState.targets = &colorTarget;

        // ---------------------------------------------
        // 5. Configure render pipeline
        // ---------------------------------------------

        WGPURenderPipelineDescriptor pipelineDescriptor{};

        pipelineDescriptor.label = WGPUStringView{"Triangle Render Pipeline", WGPU_STRLEN};

        // nullptr means automatic pipeline layout.
        pipelineDescriptor.layout = nullptr;

        // Vertex stage.
        pipelineDescriptor.vertex.module = triangleShaderModule;

        pipelineDescriptor.vertex.entryPoint = WGPUStringView{"vs_main", WGPU_STRLEN};

        // No vertex buffers.
        pipelineDescriptor.vertex.bufferCount = 0;
        pipelineDescriptor.vertex.buffers     = nullptr;

        // Primitive state.
        pipelineDescriptor.primitive.topology = WGPUPrimitiveTopology_TriangleList;

        pipelineDescriptor.primitive.stripIndexFormat = WGPUIndexFormat_Undefined;

        pipelineDescriptor.primitive.frontFace = WGPUFrontFace_CCW;

        pipelineDescriptor.primitive.cullMode = WGPUCullMode_None;

        pipelineDescriptor.primitive.unclippedDepth = WGPU_FALSE;

        // Multisampling.
        pipelineDescriptor.multisample.count = 1;

        pipelineDescriptor.multisample.mask = 0xFFFFFFFF;

        pipelineDescriptor.multisample.alphaToCoverageEnabled = WGPU_FALSE;

        // Fragment stage.
        pipelineDescriptor.fragment = &fragmentState;

        // ---------------------------------------------
        // 6. Create render pipeline
        // ---------------------------------------------

        trianglePipeline = wgpuDeviceCreateRenderPipeline(device, &pipelineDescriptor);

        if (!trianglePipeline)
        {
            LOG_ERROR << "Could not create triangle render pipeline.";

            wgpuShaderModuleRelease(triangleShaderModule);
            triangleShaderModule = nullptr;

            return false;
        }

        LOG_INFO << "Triangle render pipeline created.";

        return true;
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
    if (impl_->contextCreationStarted)
    {
        LOG_ERROR << "WebGPU context initialization was already started.";

        if (callback)
            callback(false);

        return;
    }

    impl_->contextCreationStarted = true;
    impl_->contextReadyCallback   = std::move(callback);

    // 1. Create WebGPU instance.
    impl_->instance = wgpuCreateInstance(nullptr);

    if (!impl_->instance)
    {
        impl_->FinishInitialization(false, "Could not create WebGPU instance.");

        return;
    }

    LOG_INFO << "WebGPU instance created.";

    // 2. Create surface for the HTML canvas.
    //
    // Na razie używamy selektora "canvas", tak jak w Twoim
    // poprzednim kodzie JS: document.querySelector("canvas").
    //
    // Później możemy pobierać selektor z WebGpuWindowApi.

    WGPUEmscriptenSurfaceSourceCanvasHTMLSelector canvasSource{};

    canvasSource.chain.sType = WGPUSType_EmscriptenSurfaceSourceCanvasHTMLSelector;

    canvasSource.chain.next = nullptr;

    canvasSource.selector = WGPUStringView{"canvas", WGPU_STRLEN};

    WGPUSurfaceDescriptor surfaceDescriptor{};

    surfaceDescriptor.nextInChain = &canvasSource.chain;

    surfaceDescriptor.label = WGPUStringView{"BEngine canvas surface", WGPU_STRLEN};

    impl_->surface = wgpuInstanceCreateSurface(impl_->instance, &surfaceDescriptor);

    if (!impl_->surface)
    {
        impl_->FinishInitialization(false, "Could not create WebGPU surface for canvas.");

        return;
    }

    LOG_INFO << "WebGPU surface created.";

    // 3. Request adapter compatible with our surface.
    WGPURequestAdapterOptions adapterOptions{};

    adapterOptions.compatibleSurface = impl_->surface;

    WGPURequestAdapterCallbackInfo callbackInfo{};

    callbackInfo.mode      = WGPUCallbackMode_AllowProcessEvents;
    callbackInfo.callback  = &Impl::OnAdapterRequested;
    callbackInfo.userdata1 = impl_.get();

    wgpuInstanceRequestAdapter(impl_->instance, &adapterOptions, callbackInfo);

    LOG_INFO << "WebGPU adapter request started.";
}
void WebGpuApi::ProcessEvents()
{
    if (impl_->instance)
    {
        wgpuInstanceProcessEvents(impl_->instance);
    }
}

IWindowApi& WebGpuApi::GetWindowApi()
{
    return *impl_->windowApi;
}
void WebGpuApi::Init()
{
    LOG_DEBUG << "WebGpuApi::Init";

    if (!impl_->deviceReady)
    {
        throw std::runtime_error("WebGPU device is not ready");
    }
}
void WebGpuApi::PrepareFrame()
{
    ProcessEvents();

    auto& api = *impl_;

    if (!api.deviceReady || !api.surfaceConfigured || !api.surface || !api.queue)
    {
        return;
    }

    if (api.frameInProgress)
    {
        LOG_ERROR << "Previous WebGPU frame was not finished.";
        return;
    }

    // ---------------------------------------------
    // 1. Pobranie backbuffera aktualnej klatki
    // ---------------------------------------------

    WGPUSurfaceTexture surfaceTexture{};

    wgpuSurfaceGetCurrentTexture(api.surface, &surfaceTexture);

    if (!surfaceTexture.texture)
    {
        LOG_ERROR << "Could not acquire surface texture. Status: " << static_cast<uint32>(surfaceTexture.status);

        return;
    }

    api.frameTexture = surfaceTexture.texture;

    if (surfaceTexture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessOptimal &&
        surfaceTexture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessSuboptimal)
    {
        LOG_ERROR << "Surface texture acquisition failed. Status: " << static_cast<uint32>(surfaceTexture.status);

        api.ReleaseFrameResources();
        return;
    }

    // ---------------------------------------------
    // 2. Utworzenie TextureView
    // ---------------------------------------------

    api.frameTextureView = wgpuTextureCreateView(api.frameTexture, nullptr);

    if (!api.frameTextureView)
    {
        LOG_ERROR << "Could not create surface texture view.";

        api.ReleaseFrameResources();
        return;
    }

    // ---------------------------------------------
    // 3. Utworzenie Command Encoder
    // ---------------------------------------------

    WGPUCommandEncoderDescriptor encoderDescriptor{};

    encoderDescriptor.label = WGPUStringView{"WebGPU Frame Encoder", WGPU_STRLEN};

    api.frameCommandEncoder = wgpuDeviceCreateCommandEncoder(api.device, &encoderDescriptor);

    if (!api.frameCommandEncoder)
    {
        LOG_ERROR << "Could not create command encoder.";

        api.ReleaseFrameResources();
        return;
    }

    // ---------------------------------------------
    // 4. Konfiguracja Color Attachment
    // ---------------------------------------------

    WGPURenderPassColorAttachment colorAttachment{};

    colorAttachment.view = api.frameTextureView;

    colorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;

    colorAttachment.loadOp  = WGPULoadOp_Clear;
    colorAttachment.storeOp = WGPUStoreOp_Store;

    // Testowy kolor tła.
    colorAttachment.clearValue = WGPUColor{bgColor.value.x, bgColor.value.y, bgColor.value.z, bgColor.value.w};

    // ---------------------------------------------
    // 5. Utworzenie Render Pass
    // ---------------------------------------------

    WGPURenderPassDescriptor renderPassDescriptor{};

    renderPassDescriptor.label = WGPUStringView{"WebGPU Frame Render Pass", WGPU_STRLEN};

    renderPassDescriptor.colorAttachmentCount = 1;

    renderPassDescriptor.colorAttachments = &colorAttachment;

    api.frameRenderPass = wgpuCommandEncoderBeginRenderPass(api.frameCommandEncoder, &renderPassDescriptor);

    if (!api.frameRenderPass)
    {
        LOG_ERROR << "Could not create render pass.";

        api.ReleaseFrameResources();
        return;
    }

    api.frameInProgress = true;

    // ---------------------------------------------
    // Draw test triangle
    // ---------------------------------------------

    if (api.trianglePipeline)
    {
        wgpuRenderPassEncoderSetPipeline(api.frameRenderPass, api.trianglePipeline);

        wgpuRenderPassEncoderDraw(api.frameRenderPass,
                                  3,  // Vertex count
                                  1,  // Instance count
                                  0,  // First vertex
                                  0   // First instance
        );
    }
}
void WebGpuApi::EndFrame()
{
    auto& api = *impl_;

    if (!api.frameInProgress)
        return;

    // ---------------------------------------------
    // 1. Zakończenie Render Pass
    // ---------------------------------------------

    wgpuRenderPassEncoderEnd(api.frameRenderPass);

    wgpuRenderPassEncoderRelease(api.frameRenderPass);

    api.frameRenderPass = nullptr;

    // ---------------------------------------------
    // 2. Zakończenie Command Encoder
    // ---------------------------------------------

    WGPUCommandBuffer commandBuffer = wgpuCommandEncoderFinish(api.frameCommandEncoder, nullptr);

    wgpuCommandEncoderRelease(api.frameCommandEncoder);

    api.frameCommandEncoder = nullptr;

    if (!commandBuffer)
    {
        LOG_ERROR << "Could not finish command encoder.";

        api.ReleaseFrameResources();
        return;
    }

    // ---------------------------------------------
    // 3. Wysłanie poleceń do GPU
    // ---------------------------------------------

    wgpuQueueSubmit(api.queue, 1, &commandBuffer);

    wgpuCommandBufferRelease(commandBuffer);
    // ---------------------------------------------
    // 5. Zwolnienie zasobów klatki
    // ---------------------------------------------

    api.ReleaseFrameResources();
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
    if (!impl_->deviceReady)
    {
        throw std::runtime_error("Cannot create graphics context: WebGPU device is not ready.");
    }
}
void WebGpuApi::DeleteContext()
{
    impl_->Release();

    impl_->contextCreationStarted = false;
    impl_->contextReadyCallback   = {};
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
