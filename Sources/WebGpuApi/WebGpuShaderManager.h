#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>

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
};

class WebGpuShaderManager
{
public:
    WebGpuShaderManager();

    void SetShadersFilesLocations(const std::filesystem::path& path);
    void SetShaderQuaility(GraphicsApi::ShaderQuaility quality);

    ID Create(GraphicsApi::ShaderProgramType shaderType);
    void UseShader(uint32 id);
    void DeleteShader(uint32 id);

private:
    std::optional<GraphicsApi::ShadersFiles> GetShaderFiles(GraphicsApi::ShaderProgramType shaderType) const;

private:
    uint32 nextShaderId_ = 1;
    uint32 currentShader_ = 0;
    std::filesystem::path shadersFileLocation_;
    GraphicsApi::ShaderQuaility shaderQuality_{GraphicsApi::ShaderQuaility::SimpleForwardRendering};
    std::unordered_map<uint32, WebGpuShaderProgram> shaderPrograms_;
};

}  // namespace GraphicsApi::WebGpu
