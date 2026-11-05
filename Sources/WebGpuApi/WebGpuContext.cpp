#include "WebGpuContext.h"

#include <utility>

#include <Logger/Log.h>

namespace GraphicsApi::WebGpu
{
WebGpuContext::WebGpuContext() = default;

WebGpuContext::~WebGpuContext()
{
    Release();
}

void WebGpuContext::SetReadyCallback(std::function<void(bool)> callback)
{
    contextReadyCallback = std::move(callback);
}

bool WebGpuContext::CreateInstance()
{
    instance = wgpuCreateInstance(nullptr);
    return instance != nullptr;
}

bool WebGpuContext::CreateCanvasSurface()
{
    if (!instance)
    {
        return false;
    }

    WGPUEmscriptenSurfaceSourceCanvasHTMLSelector canvasSource{};
    canvasSource.chain.sType = WGPUSType_EmscriptenSurfaceSourceCanvasHTMLSelector;
    canvasSource.chain.next  = nullptr;
    canvasSource.selector    = WGPUStringView{"canvas", WGPU_STRLEN};

    WGPUSurfaceDescriptor surfaceDescriptor{};
    surfaceDescriptor.nextInChain = &canvasSource.chain;
    surfaceDescriptor.label       = WGPUStringView{"BEngine canvas surface", WGPU_STRLEN};

    surface = wgpuInstanceCreateSurface(instance, &surfaceDescriptor);
    return surface != nullptr;
}

bool WebGpuContext::RequestAdapter()
{
    if (!surface)
    {
        return false;
    }

    WGPURequestAdapterOptions adapterOptions{};
    adapterOptions.compatibleSurface = surface;

    WGPURequestAdapterCallbackInfo callbackInfo{};
    callbackInfo.mode      = WGPUCallbackMode_AllowProcessEvents;
    callbackInfo.callback  = &WebGpuContext::OnAdapterRequested;
    callbackInfo.userdata1 = this;

    wgpuInstanceRequestAdapter(instance, &adapterOptions, callbackInfo);
    return true;
}

bool WebGpuContext::ConfigureSurface()
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

    WGPUSurfaceConfiguration config{};
    config.device = device;
    config.format = capabilities.formats[0];
    config.usage  = WGPUTextureUsage_RenderAttachment;
    config.width  = static_cast<uint32>(width);
    config.height = static_cast<uint32>(height);
    config.presentMode = WGPUPresentMode_Fifo;
    config.alphaMode    = WGPUCompositeAlphaMode_Auto;

    wgpuSurfaceConfigure(surface, &config);
    wgpuSurfaceCapabilitiesFreeMembers(capabilities);

    surfaceFormat = config.format;
    surfaceWidth  = config.width;
    surfaceHeight = config.height;
    surfaceConfigured = true;

    LOG_INFO << "WebGPU surface configured: " << surfaceWidth << "x" << surfaceHeight;
    return true;
}

void WebGpuContext::ProcessEvents()
{
    if (instance)
    {
        wgpuInstanceProcessEvents(instance);
    }
}

void WebGpuContext::ReleaseFrameResources()
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

void WebGpuContext::Release()
{
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

std::string WebGpuContext::GetMessage(WGPUStringView message)
{
    if (!message.data)
    {
        return {};
    }

    if (message.length == WGPU_STRLEN)
    {
        return message.data;
    }

    return std::string(message.data, message.length);
}

void WebGpuContext::FinishInitialization(bool success, const std::string& message)
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

void WebGpuContext::OnAdapterRequested(WGPURequestAdapterStatus status, WGPUAdapter requestedAdapter,
                                      WGPUStringView message, void* userdata1, void*)
{
    auto* self = static_cast<WebGpuContext*>(userdata1);
    if (!self)
    {
        return;
    }

    if (status != WGPURequestAdapterStatus_Success || !requestedAdapter)
    {
        self->FinishInitialization(false, GetMessage(message));
        return;
    }

    self->adapter = requestedAdapter;
    LOG_INFO << "WebGPU adapter acquired.";

    WGPUDeviceDescriptor deviceDescriptor{};
    WGPURequestDeviceCallbackInfo callbackInfo{};
    callbackInfo.mode      = WGPUCallbackMode_AllowProcessEvents;
    callbackInfo.callback  = &WebGpuContext::OnDeviceRequested;
    callbackInfo.userdata1 = self;

    wgpuAdapterRequestDevice(self->adapter, &deviceDescriptor, callbackInfo);
}

void WebGpuContext::OnDeviceRequested(WGPURequestDeviceStatus status, WGPUDevice requestedDevice,
                                      WGPUStringView message, void* userdata1, void*)
{
    auto* self = static_cast<WebGpuContext*>(userdata1);
    if (!self)
    {
        return;
    }

    if (status != WGPURequestDeviceStatus_Success || !requestedDevice)
    {
        self->FinishInitialization(false, GetMessage(message));
        return;
    }

    self->device = requestedDevice;
    LOG_INFO << "WebGPU device acquired.";

    self->queue = wgpuDeviceGetQueue(self->device);
    if (!self->queue)
    {
        self->FinishInitialization(false, "Could not acquire WebGPU queue.");
        return;
    }

    if (!self->ConfigureSurface())
    {
        self->FinishInitialization(false, "Could not configure WebGPU surface.");
        return;
    }

    self->FinishInitialization(true);
}

bool WebGpuContext::IsReady() const
{
    return deviceReady;
}

bool WebGpuContext::IsConfigured() const
{
    return surfaceConfigured;
}

bool WebGpuContext::IsCreationStarted() const
{
    return contextCreationStarted;
}

bool WebGpuContext::HasInstance() const
{
    return instance != nullptr;
}

}  // namespace GraphicsApi::WebGpu
