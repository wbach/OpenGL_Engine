#include "WebGpuTriangleRenderer.h"

#include <Logger/Log.h>

namespace GraphicsApi::WebGpu
{
namespace
{
constexpr static char kTriangleShader[] = R"WGSL(
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

        return vec4<f32>(positions[vertexIndex], 0.0, 1.0);
    }

    @fragment
    fn fs_main() -> @location(0) vec4<f32>
    {
        return vec4<f32>(0.1, 0.9, 0.25, 1.0);
    }
)WGSL";

constexpr static char kTriangleVertexShader[] = R"WGSL(
    struct VertexInput
    {
        @location(0) position: vec3<f32>,
    };

    @vertex
    fn vs_main(input: VertexInput) -> @builtin(position) vec4<f32>
    {
        return vec4<f32>(input.position, 1.0);
    }

    @fragment
    fn fs_main() -> @location(0) vec4<f32>
    {
        return vec4<f32>(0.1, 0.1, 0.25, 1.0);
    }
)WGSL";

}  // namespace

WebGpuTriangleRenderer::~WebGpuTriangleRenderer()
{
    Release();
}

bool WebGpuTriangleRenderer::Initialize(WGPUDevice device, WGPUTextureFormat surfaceFormat, WGPUQueue queue)
{
    if (!device || surfaceFormat == WGPUTextureFormat_Undefined)
    {
        return false;
    }

    device_ = device;
    queue_  = queue;
    surfaceFormat_ = surfaceFormat;

    if (!CreateTrianglePipeline())
    {
        return false;
    }

    if (!CreateTriangleVertexBuffer())
    {
        return false;
    }

    if (!CreateTriangleVertexBufferPipeline())
    {
        return false;
    }

    return true;
}

bool WebGpuTriangleRenderer::Draw(WGPURenderPassEncoder renderPass) const
{
    if (!renderPass || !triangleVertexBufferPipeline || !triangleVertexBuffer)
    {
        return false;
    }

    wgpuRenderPassEncoderSetPipeline(renderPass, triangleVertexBufferPipeline);
    wgpuRenderPassEncoderSetVertexBuffer(renderPass, 0, triangleVertexBuffer, 0, sizeof(float) * 9);
    wgpuRenderPassEncoderDraw(renderPass, 3, 1, 0, 0);
    return true;
}

void WebGpuTriangleRenderer::Release()
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

    if (triangleVertexBufferPipeline)
    {
        wgpuRenderPipelineRelease(triangleVertexBufferPipeline);
        triangleVertexBufferPipeline = nullptr;
    }

    if (triangleVertexBufferShaderModule)
    {
        wgpuShaderModuleRelease(triangleVertexBufferShaderModule);
        triangleVertexBufferShaderModule = nullptr;
    }

    if (triangleVertexBuffer)
    {
        wgpuBufferDestroy(triangleVertexBuffer);
        wgpuBufferRelease(triangleVertexBuffer);
        triangleVertexBuffer = nullptr;
    }
}

bool WebGpuTriangleRenderer::IsReady() const
{
    return triangleVertexBufferPipeline != nullptr && triangleVertexBuffer != nullptr;
}

bool WebGpuTriangleRenderer::CreateTrianglePipeline()
{
    WGPUShaderSourceWGSL wgslSource{};
    wgslSource.chain.sType = WGPUSType_ShaderSourceWGSL;
    wgslSource.chain.next  = nullptr;
    wgslSource.code = WGPUStringView{kTriangleShader, WGPU_STRLEN};

    WGPUShaderModuleDescriptor shaderDescriptor{};
    shaderDescriptor.nextInChain = &wgslSource.chain;
    shaderDescriptor.label = WGPUStringView{"Triangle Shader", WGPU_STRLEN};

    triangleShaderModule = wgpuDeviceCreateShaderModule(device_, &shaderDescriptor);
    if (!triangleShaderModule)
    {
        LOG_ERROR << "Could not create triangle shader module.";
        return false;
    }

    WGPUColorTargetState colorTarget{};
    colorTarget.format = surfaceFormat_;
    colorTarget.writeMask = WGPUColorWriteMask_All;

    WGPUFragmentState fragmentState{};
    fragmentState.module = triangleShaderModule;
    fragmentState.entryPoint = WGPUStringView{"fs_main", WGPU_STRLEN};
    fragmentState.targetCount = 1;
    fragmentState.targets = &colorTarget;

    WGPURenderPipelineDescriptor pipelineDescriptor{};
    pipelineDescriptor.label = WGPUStringView{"Triangle Render Pipeline", WGPU_STRLEN};
    pipelineDescriptor.layout = nullptr;
    pipelineDescriptor.vertex.module = triangleShaderModule;
    pipelineDescriptor.vertex.entryPoint = WGPUStringView{"vs_main", WGPU_STRLEN};
    pipelineDescriptor.vertex.bufferCount = 0;
    pipelineDescriptor.vertex.buffers = nullptr;
    pipelineDescriptor.primitive.topology = WGPUPrimitiveTopology_TriangleList;
    pipelineDescriptor.primitive.stripIndexFormat = WGPUIndexFormat_Undefined;
    pipelineDescriptor.primitive.frontFace = WGPUFrontFace_CCW;
    pipelineDescriptor.primitive.cullMode = WGPUCullMode_None;
    pipelineDescriptor.primitive.unclippedDepth = WGPU_FALSE;
    pipelineDescriptor.multisample.count = 1;
    pipelineDescriptor.multisample.mask = 0xFFFFFFFF;
    pipelineDescriptor.multisample.alphaToCoverageEnabled = WGPU_FALSE;
    pipelineDescriptor.fragment = &fragmentState;

    trianglePipeline = wgpuDeviceCreateRenderPipeline(device_, &pipelineDescriptor);
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

bool WebGpuTriangleRenderer::CreateTriangleVertexBuffer()
{
    static constexpr float vertices[] = {0.0f, 0.7f, 0.0f, -0.7f, -0.65f, 0.0f, 0.7f, -0.65f, 0.0f};

    WGPUBufferDescriptor descriptor{};
    descriptor.label = WGPUStringView{"Triangle Vertex Buffer", WGPU_STRLEN};
    descriptor.size = sizeof(vertices);
    descriptor.usage = WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst;
    descriptor.mappedAtCreation = WGPU_FALSE;

    triangleVertexBuffer = wgpuDeviceCreateBuffer(device_, &descriptor);
    if (!triangleVertexBuffer)
    {
        LOG_ERROR << "Could not create triangle vertex buffer.";
        return false;
    }

    wgpuQueueWriteBuffer(queue_, triangleVertexBuffer, 0, vertices, sizeof(vertices));
    LOG_INFO << "Triangle vertex buffer created.";
    return true;
}

bool WebGpuTriangleRenderer::CreateTriangleVertexBufferPipeline()
{
    WGPUShaderSourceWGSL wgslSource{};
    wgslSource.chain.sType = WGPUSType_ShaderSourceWGSL;
    wgslSource.chain.next = nullptr;
    wgslSource.code = WGPUStringView{kTriangleVertexShader, WGPU_STRLEN};

    WGPUShaderModuleDescriptor shaderDescriptor{};
    shaderDescriptor.nextInChain = &wgslSource.chain;
    shaderDescriptor.label = WGPUStringView{"Triangle Vertex Buffer Shader", WGPU_STRLEN};

    triangleVertexBufferShaderModule = wgpuDeviceCreateShaderModule(device_, &shaderDescriptor);
    if (!triangleVertexBufferShaderModule)
    {
        LOG_ERROR << "Could not create triangle vertex buffer shader.";
        return false;
    }

    WGPUVertexAttribute positionAttribute{};
    positionAttribute.format = WGPUVertexFormat_Float32x3;
    positionAttribute.offset = 0;
    positionAttribute.shaderLocation = 0;

    WGPUVertexBufferLayout vertexBufferLayout{};
    vertexBufferLayout.arrayStride = sizeof(float) * 3;
    vertexBufferLayout.stepMode = WGPUVertexStepMode_Vertex;
    vertexBufferLayout.attributeCount = 1;
    vertexBufferLayout.attributes = &positionAttribute;

    WGPUVertexState vertexState{};
    vertexState.module = triangleVertexBufferShaderModule;
    vertexState.entryPoint = WGPUStringView{"vs_main", WGPU_STRLEN};
    vertexState.bufferCount = 1;
    vertexState.buffers = &vertexBufferLayout;

    WGPUColorTargetState colorTarget{};
    colorTarget.format = surfaceFormat_;
    colorTarget.writeMask = WGPUColorWriteMask_All;

    WGPUFragmentState fragmentState{};
    fragmentState.module = triangleVertexBufferShaderModule;
    fragmentState.entryPoint = WGPUStringView{"fs_main", WGPU_STRLEN};
    fragmentState.targetCount = 1;
    fragmentState.targets = &colorTarget;

    WGPURenderPipelineDescriptor pipelineDescriptor{};
    pipelineDescriptor.label = WGPUStringView{"Triangle Vertex Buffer Pipeline", WGPU_STRLEN};
    pipelineDescriptor.layout = nullptr;
    pipelineDescriptor.vertex = vertexState;
    pipelineDescriptor.primitive.topology = WGPUPrimitiveTopology_TriangleList;
    pipelineDescriptor.primitive.stripIndexFormat = WGPUIndexFormat_Undefined;
    pipelineDescriptor.primitive.frontFace = WGPUFrontFace_CCW;
    pipelineDescriptor.primitive.cullMode = WGPUCullMode_None;
    pipelineDescriptor.primitive.unclippedDepth = WGPU_FALSE;
    pipelineDescriptor.multisample.count = 1;
    pipelineDescriptor.multisample.mask = 0xFFFFFFFF;
    pipelineDescriptor.multisample.alphaToCoverageEnabled = WGPU_FALSE;
    pipelineDescriptor.fragment = &fragmentState;

    triangleVertexBufferPipeline = wgpuDeviceCreateRenderPipeline(device_, &pipelineDescriptor);
    if (!triangleVertexBufferPipeline)
    {
        LOG_ERROR << "Could not create triangle vertex buffer pipeline.";
        wgpuShaderModuleRelease(triangleVertexBufferShaderModule);
        triangleVertexBufferShaderModule = nullptr;
        return false;
    }

    LOG_INFO << "Triangle vertex buffer pipeline created.";
    return true;
}

}  // namespace GraphicsApi::WebGpu
