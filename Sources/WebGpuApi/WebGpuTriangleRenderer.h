#pragma once

#include <webgpu/webgpu.h>

namespace GraphicsApi::WebGpu
{
class WebGpuTriangleRenderer
{
public:
    WebGpuTriangleRenderer() = default;
    ~WebGpuTriangleRenderer();

    bool Initialize(WGPUDevice device, WGPUTextureFormat surfaceFormat, WGPUQueue queue);
    bool Draw(WGPURenderPassEncoder renderPass) const;
    void Release();

    bool IsReady() const;

private:
    bool CreateTrianglePipeline();
    bool CreateTriangleVertexBuffer();
    bool CreateTriangleVertexBufferPipeline();

    WGPUDevice device_ = nullptr;
    WGPUQueue queue_   = nullptr;
    WGPUTextureFormat surfaceFormat_ = WGPUTextureFormat_Undefined;

    WGPUShaderModule triangleShaderModule = nullptr;
    WGPURenderPipeline trianglePipeline   = nullptr;

    WGPUShaderModule triangleVertexBufferShaderModule = nullptr;
    WGPURenderPipeline triangleVertexBufferPipeline   = nullptr;

    WGPUBuffer triangleVertexBuffer = nullptr;
};

}  // namespace GraphicsApi::WebGpu
