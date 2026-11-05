#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include <webgpu/webgpu.h>

#include "GraphicsApi/GraphicsApiDef.h"
#include "GraphicsApi/ShaderFiles.h"
#include "GraphicsApi/ShaderProgramType.h"
#include "GraphicsApi/ShaderQuaility.h"
#include "GraphicsApi/ShaderTypes.h"

namespace GraphicsApi::WebGpu
{
struct WebGpuShaderProgram
{
    ID id{std::nullopt};
    std::string name;
    GraphicsApi::ShaderProgramType type{GraphicsApi::ShaderProgramType::Loading};
    GraphicsApi::ShadersFiles sources;
    WGPUShaderModule vertexModule = nullptr;
    WGPUShaderModule fragmentModule = nullptr;
    WGPUPipelineLayout pipelineLayout = nullptr;
    WGPUBindGroupLayout bindGroupLayout = nullptr;
    WGPURenderPipeline pipeline = nullptr;
    WGPUBindGroup bindGroup = nullptr;
    std::vector<WGPUBindGroupLayoutEntry> requiredBindings;
};

class WebGpuShaderManager
{
public:
    WebGpuShaderManager();
    ~WebGpuShaderManager();

    void SetDevice(WGPUDevice device, WGPUTextureFormat surfaceFormat);
    void SetShadersFilesLocations(const std::filesystem::path& path);
    void SetShaderQuaility(GraphicsApi::ShaderQuaility quality);

    ID Create(GraphicsApi::ShaderProgramType shaderType);
    void UseShader(uint32 id);
    void DeleteShader(uint32 id);
    void Release();
    uint32 GetCurrentProgramId() const;
    WebGpuShaderProgram* GetProgram(uint32 id);
    const WebGpuShaderProgram* GetProgram(uint32 id) const;

private:
    bool CreatePipelineForProgram(WebGpuShaderProgram& program);
    std::optional<GraphicsApi::ShadersFiles> GetShaderFiles(GraphicsApi::ShaderProgramType shaderType) const;

private:
    uint32 nextShaderId_ = 1;
    uint32 currentShader_ = 0;
    WGPUDevice device_ = nullptr;
    WGPUTextureFormat surfaceFormat_ = WGPUTextureFormat_Undefined;
    std::filesystem::path shadersFileLocation_;
    GraphicsApi::ShaderQuaility shaderQuality_{GraphicsApi::ShaderQuaility::SimpleForwardRendering};
    std::unordered_map<uint32, WebGpuShaderProgram> shaderPrograms_;
};

}  // namespace GraphicsApi::WebGpu
