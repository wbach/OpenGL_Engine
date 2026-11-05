#include "WebGpuShaderManager.h"

#include <Logger/Log.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <magic_enum/magic_enum.hpp>

namespace GraphicsApi::WebGpu
{
namespace
{
constexpr const char kDefaultVertexShader[] = R"WGSL(
struct VertexInput {
    @location(0) position: vec3<f32>,
    @location(1) uv: vec2<f32>,
};

struct VertexOutput {
    @builtin(position) position: vec4<f32>,
    @location(0) uv: vec2<f32>,
};

@vertex
fn vs_main(input: VertexInput) -> VertexOutput {
    var output: VertexOutput;
    output.position = vec4<f32>(input.position, 1.0);
    output.uv = input.uv;
    return output;
}
)WGSL";

constexpr const char kDefaultFragmentShader[] = R"WGSL(
@fragment
fn fs_main(@location(0) uv: vec2<f32>) -> @location(0) vec4<f32> {
    return vec4<f32>(0.2, 0.8, 1.0, 1.0);
}
)WGSL";

const std::unordered_map<GraphicsApi::ShaderProgramType, GraphicsApi::ShadersFiles> simpleForwardShaderFiles = {
    {GraphicsApi::ShaderProgramType::Loading,
     {{GraphicsApi::ShaderType::VERTEX_SHADER, "Common/Quad.vert"},
      {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Game/loadingShader.frag"}}},
    {GraphicsApi::ShaderProgramType::Texture,
     {{GraphicsApi::ShaderType::VERTEX_SHADER, "Gui/guiFragmentShader.vert"},
      {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Gui/guiFragmentShader.frag"}}},
    {GraphicsApi::ShaderProgramType::Grid,
     {{GraphicsApi::ShaderType::VERTEX_SHADER, "DebugShaders/gridShader.vert"},
      {GraphicsApi::ShaderType::FRAGMENT_SHADER, "DebugShaders/gridShader.frag"}}},
    {GraphicsApi::ShaderProgramType::Line,
     {{GraphicsApi::ShaderType::VERTEX_SHADER, "DebugShaders/lineDebugShader.vert"},
      {GraphicsApi::ShaderType::FRAGMENT_SHADER, "DebugShaders/lineDebugShader.frag"}}},
    {GraphicsApi::ShaderProgramType::DebugObject,
     {{GraphicsApi::ShaderType::VERTEX_SHADER, "DebugShaders/debugObjectShader.vert"},
      {GraphicsApi::ShaderType::FRAGMENT_SHADER, "DebugShaders/debugObjectShader.frag"}}},
    {GraphicsApi::ShaderProgramType::DebugNormal,
     {{GraphicsApi::ShaderType::VERTEX_SHADER, "DebugShaders/NormalShader.vert"},
      {GraphicsApi::ShaderType::FRAGMENT_SHADER, "DebugShaders/NormalShader.frag"}}},
    {GraphicsApi::ShaderProgramType::Entity,
     {{GraphicsApi::ShaderType::VERTEX_SHADER, "Entity/Forward/SimpleEntityShader.vert"},
      {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Entity/Forward/SimpleEntityShader.frag"}}},
    {GraphicsApi::ShaderProgramType::TerrainMesh,
     {{GraphicsApi::ShaderType::VERTEX_SHADER, "Terrain/Mesh/TerrainShader.vert"},
      {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Terrain/Mesh/TerrainShader.frag"}}},
    {GraphicsApi::ShaderProgramType::TreeLeafs,
     {{GraphicsApi::ShaderType::VERTEX_SHADER, "Tree/LeafShader.vert"},
      {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Tree/Forward/LeafShader.frag"}}},
    {GraphicsApi::ShaderProgramType::TreeLeafsCluster,
     {{GraphicsApi::ShaderType::VERTEX_SHADER, "Tree/LeafClusterShader.vert"},
      {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Tree/Forward/LeafClusterShader.frag"}}},
    {GraphicsApi::ShaderProgramType::Particles,
     {{GraphicsApi::ShaderType::VERTEX_SHADER, "Particles/Forward/ParticlesShader.vert"},
      {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Particles/Forward/ParticlesShader.frag"}}}
};

std::optional<GraphicsApi::ShadersFiles> GetSimpleForwardShaderFiles(GraphicsApi::ShaderProgramType shaderType)
{
    const auto it = simpleForwardShaderFiles.find(shaderType);
    if (it == simpleForwardShaderFiles.end())
    {
        return std::nullopt;
    }

    return it->second;
}

std::optional<GraphicsApi::ShadersFiles> GetFullDefferedShaderFiles(GraphicsApi::ShaderProgramType shaderType)
{
    (void)shaderType;
    return std::nullopt;
}
}  // namespace

WebGpuShaderManager::WebGpuShaderManager()
    : shaderQuality_(GraphicsApi::ShaderQuaility::SimpleForwardRendering)
{
}

WebGpuShaderManager::~WebGpuShaderManager()
{
    Release();
}

void WebGpuShaderManager::SetDevice(WGPUDevice device, WGPUTextureFormat surfaceFormat)
{
    device_ = device;
    surfaceFormat_ = surfaceFormat;
}

void WebGpuShaderManager::SetShadersFilesLocations(const std::filesystem::path& path)
{
    shadersFileLocation_ = path / "GLSL";
    LOG_DEBUG << "WebGPU shader path: " << shadersFileLocation_;
}

void WebGpuShaderManager::SetShaderQuaility(GraphicsApi::ShaderQuaility quality)
{
    shaderQuality_ = quality;
}

ID WebGpuShaderManager::Create(GraphicsApi::ShaderProgramType shaderType)
{
    if (!device_)
    {
        LOG_WARN << "WebGPU shader manager device is not initialized yet.";
        return std::nullopt;
    }

    LOG_DEBUG << "Create WebGPU shader, type: " << magic_enum::enum_name(shaderType);

    const uint32 shaderId = nextShaderId_++;
    WebGpuShaderProgram program{};
    program.id   = shaderId;
    program.type = shaderType;
    program.name = magic_enum::enum_name(shaderType);

    const auto files = GetShaderFiles(shaderType);
    if (files)
    {
        for (const auto& [stage, fileName] : *files)
        {
            const auto fullPath = std::filesystem::absolute(shadersFileLocation_ / fileName).lexically_normal();
            const std::string source = Utils::ReadFilesWithIncludes(fullPath.string());
            if (source.empty())
            {
                LOG_WARN << "Shader source is empty for " << fileName << " (path: " << fullPath << ")";
                continue;
            }

            program.sources[stage] = source;
        }
    }

    if (program.sources.empty())
    {
        program.sources[GraphicsApi::ShaderType::VERTEX_SHADER] = kDefaultVertexShader;
        program.sources[GraphicsApi::ShaderType::FRAGMENT_SHADER] = kDefaultFragmentShader;
    }

    if (!CreatePipelineForProgram(program))
    {
        LOG_ERROR << "Failed to create WebGPU pipeline for shader type: " << magic_enum::enum_name(shaderType);
        return std::nullopt;
    }

    shaderPrograms_[shaderId] = program;
    currentShader_ = shaderId;

    LOG_DEBUG << "WebGPU shader successfully created. Id: " << shaderId;
    return shaderId;
}

void WebGpuShaderManager::UseShader(uint32 id)
{
    const auto it = shaderPrograms_.find(id);
    if (it == shaderPrograms_.end())
    {
        return;
    }

    currentShader_ = id;
}

void WebGpuShaderManager::DeleteShader(uint32 id)
{
    auto it = shaderPrograms_.find(id);
    if (it == shaderPrograms_.end())
    {
        return;
    }

    if (it->second.pipeline)
    {
        wgpuRenderPipelineRelease(it->second.pipeline);
    }

    if (it->second.vertexModule)
    {
        wgpuShaderModuleRelease(it->second.vertexModule);
    }

    if (it->second.fragmentModule)
    {
        wgpuShaderModuleRelease(it->second.fragmentModule);
    }

    shaderPrograms_.erase(it);
    if (currentShader_ == id)
    {
        currentShader_ = 0;
    }
}

void WebGpuShaderManager::Release()
{
    for (auto& [id, program] : shaderPrograms_)
    {
        if (program.pipeline)
        {
            wgpuRenderPipelineRelease(program.pipeline);
            program.pipeline = nullptr;
        }

        if (program.vertexModule)
        {
            wgpuShaderModuleRelease(program.vertexModule);
            program.vertexModule = nullptr;
        }

        if (program.fragmentModule)
        {
            wgpuShaderModuleRelease(program.fragmentModule);
            program.fragmentModule = nullptr;
        }

        (void)id;
    }

    shaderPrograms_.clear();
    currentShader_ = 0;
}

bool WebGpuShaderManager::CreatePipelineForProgram(WebGpuShaderProgram& program)
{
    auto findSource = [&](GraphicsApi::ShaderType shaderType) -> const std::string& {
        const auto it = program.sources.find(shaderType);
        if (it != program.sources.end())
        {
            return it->second;
        }

        static const std::string fallbackVertex = kDefaultVertexShader;
        static const std::string fallbackFragment = kDefaultFragmentShader;
        if (shaderType == GraphicsApi::ShaderType::VERTEX_SHADER)
        {
            return fallbackVertex;
        }
        return fallbackFragment;
    };

    const std::string& vertexSource = findSource(GraphicsApi::ShaderType::VERTEX_SHADER);
    const std::string& fragmentSource = findSource(GraphicsApi::ShaderType::FRAGMENT_SHADER);

    WGPUShaderSourceWGSL vertexWgsl{};
    vertexWgsl.chain.sType = WGPUSType_ShaderSourceWGSL;
    vertexWgsl.code = WGPUStringView{vertexSource.c_str(), vertexSource.size()};

    WGPUShaderModuleDescriptor vertexModuleDesc{};
    vertexModuleDesc.label = WGPUStringView{"WebGPU Vertex Shader", WGPU_STRLEN};
    vertexModuleDesc.nextInChain = &vertexWgsl.chain;
    program.vertexModule = wgpuDeviceCreateShaderModule(device_, &vertexModuleDesc);
    if (!program.vertexModule)
    {
        LOG_ERROR << "Could not create WebGPU vertex shader module.";
        return false;
    }

    WGPUShaderSourceWGSL fragmentWgsl{};
    fragmentWgsl.chain.sType = WGPUSType_ShaderSourceWGSL;
    fragmentWgsl.code = WGPUStringView{fragmentSource.c_str(), fragmentSource.size()};

    WGPUShaderModuleDescriptor fragmentModuleDesc{};
    fragmentModuleDesc.label = WGPUStringView{"WebGPU Fragment Shader", WGPU_STRLEN};
    fragmentModuleDesc.nextInChain = &fragmentWgsl.chain;
    program.fragmentModule = wgpuDeviceCreateShaderModule(device_, &fragmentModuleDesc);
    if (!program.fragmentModule)
    {
        LOG_ERROR << "Could not create WebGPU fragment shader module.";
        wgpuShaderModuleRelease(program.vertexModule);
        program.vertexModule = nullptr;
        return false;
    }

    WGPUVertexAttribute positionAttribute{};
    positionAttribute.format = WGPUVertexFormat_Float32x3;
    positionAttribute.offset = 0;
    positionAttribute.shaderLocation = 0;

    WGPUVertexAttribute uvAttribute{};
    uvAttribute.format = WGPUVertexFormat_Float32x2;
    uvAttribute.offset = sizeof(float) * 3;
    uvAttribute.shaderLocation = 1;

    WGPUVertexBufferLayout vertexLayout{};
    vertexLayout.arrayStride = sizeof(float) * 5;
    vertexLayout.stepMode = WGPUVertexStepMode_Vertex;
    vertexLayout.attributeCount = 2;
    vertexLayout.attributes = &positionAttribute;

    WGPUColorTargetState colorTarget{};
    colorTarget.format = surfaceFormat_;
    colorTarget.writeMask = WGPUColorWriteMask_All;

    WGPUFragmentState fragmentState{};
    fragmentState.module = program.fragmentModule;
    fragmentState.entryPoint = WGPUStringView{"fs_main", WGPU_STRLEN};
    fragmentState.targetCount = 1;
    fragmentState.targets = &colorTarget;

    WGPURenderPipelineDescriptor pipelineDescriptor{};
    pipelineDescriptor.label = WGPUStringView{"WebGPU Shader Pipeline", WGPU_STRLEN};
    pipelineDescriptor.layout = nullptr;
    pipelineDescriptor.vertex.module = program.vertexModule;
    pipelineDescriptor.vertex.entryPoint = WGPUStringView{"vs_main", WGPU_STRLEN};
    pipelineDescriptor.vertex.bufferCount = 1;
    pipelineDescriptor.vertex.buffers = &vertexLayout;
    pipelineDescriptor.primitive.topology = WGPUPrimitiveTopology_TriangleList;
    pipelineDescriptor.primitive.stripIndexFormat = WGPUIndexFormat_Undefined;
    pipelineDescriptor.primitive.frontFace = WGPUFrontFace_CCW;
    pipelineDescriptor.primitive.cullMode = WGPUCullMode_None;
    pipelineDescriptor.multisample.count = 1;
    pipelineDescriptor.multisample.mask = 0xFFFFFFFF;
    pipelineDescriptor.multisample.alphaToCoverageEnabled = WGPU_FALSE;
    pipelineDescriptor.fragment = &fragmentState;

    program.pipeline = wgpuDeviceCreateRenderPipeline(device_, &pipelineDescriptor);
    if (!program.pipeline)
    {
        LOG_ERROR << "Could not create WebGPU render pipeline for shader type: " << magic_enum::enum_name(program.type);
        wgpuShaderModuleRelease(program.fragmentModule);
        program.fragmentModule = nullptr;
        wgpuShaderModuleRelease(program.vertexModule);
        program.vertexModule = nullptr;
        return false;
    }

    return true;
}

std::optional<GraphicsApi::ShadersFiles> WebGpuShaderManager::GetShaderFiles(GraphicsApi::ShaderProgramType shaderType) const
{
    switch (shaderQuality_)
    {
        case GraphicsApi::ShaderQuaility::SimpleForwardRendering:
            return GetSimpleForwardShaderFiles(shaderType);
        case GraphicsApi::ShaderQuaility::FullDefferedRendering:
            return GetFullDefferedShaderFiles(shaderType);
    }

    return std::nullopt;
}

}  // namespace GraphicsApi::WebGpu
