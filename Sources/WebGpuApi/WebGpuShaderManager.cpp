#include "WebGpuShaderManager.h"

#include <Logger/Log.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <magic_enum/magic_enum.hpp>

namespace GraphicsApi::WebGpu
{
namespace
{
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
    LOG_DEBUG << "Create WebGPU shader, type: " << magic_enum::enum_name(shaderType);

    const auto files = GetShaderFiles(shaderType);
    if (!files)
    {
        LOG_ERROR << "Expected shader files not found for type " << magic_enum::enum_name(shaderType)
                  << " and quality " << magic_enum::enum_name(shaderQuality_);
        return std::nullopt;
    }

    const uint32 shaderId = nextShaderId_++;
    WebGpuShaderProgram program{};
    program.id   = shaderId;
    program.type = shaderType;
    program.name = magic_enum::enum_name(shaderType);

    for (const auto& [stage, fileName] : *files)
    {
        const auto fullPath = std::filesystem::absolute(shadersFileLocation_ / fileName).lexically_normal();
        const std::string source = Utils::ReadFilesWithIncludes(fullPath.string());
        if (source.empty())
        {
            LOG_WARN << "Shader source is empty for " << fileName << " (path: " << fullPath << ")";
        }

        program.sources[stage] = source;
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
    shaderPrograms_.erase(id);
    if (currentShader_ == id)
    {
        currentShader_ = 0;
    }
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
