#pragma once

#include <Types.h>
#include <emscripten/html5.h>
#include <webgpu/webgpu.h>

#include <functional>
#include <string>

namespace GraphicsApi::WebGpu
{
class WebGpuContext
{
public:
    WebGpuContext();
    ~WebGpuContext();

    void SetReadyCallback(std::function<void(bool)> callback);
    bool CreateInstance();
    bool CreateCanvasSurface();
    bool RequestAdapter();
    bool ConfigureSurface();
    void ProcessEvents();
    void FinishInitialization(bool success, const std::string& message = {});
    void Release();
    void ReleaseFrameResources();

    bool IsReady() const;
    bool IsConfigured() const;
    bool IsCreationStarted() const;
    bool HasInstance() const;

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

    bool frameInProgress = false;

    uint32 surfaceWidth  = 0;
    uint32 surfaceHeight = 0;

    bool surfaceConfigured = false;

    bool deviceReady            = false;
    bool contextCreationStarted = false;

    std::function<void(bool)> contextReadyCallback;

private:
    static std::string GetMessage(WGPUStringView message);

    static void OnAdapterRequested(WGPURequestAdapterStatus status, WGPUAdapter requestedAdapter, WGPUStringView message,
                                   void* userdata1, void*);
    static void OnDeviceRequested(WGPURequestDeviceStatus status, WGPUDevice requestedDevice, WGPUStringView message,
                                  void* userdata1, void*);
};

}  // namespace GraphicsApi::WebGpu
