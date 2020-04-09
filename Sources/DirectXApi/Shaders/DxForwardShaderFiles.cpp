#include "DxForwardShaderFiles.h"

namespace DirectX
{
namespace
{
// clang-format off
const std::unordered_map<GraphicsApi::ShaderProgramType, GraphicsApi::ShadersFiles> shaderFiles =
{
    {
        GraphicsApi::ShaderProgramType::Loading,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Loading/LoadingShader.fx"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Loading/LoadingShader.fx"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::Entity,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Mesh/Forward/MeshShader.fx"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Mesh/Forward/MeshShader.fx"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::Texture,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Gui/TextureShader.fx"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Gui/TextureShader.fx"}
        }
    }, 
    {
        GraphicsApi::ShaderProgramType::Skydome,
        {
            { GraphicsApi::ShaderType::VERTEX_SHADER, "Skydome/Forward/SkydomeShader.fx"},
            { GraphicsApi::ShaderType::FRAGMENT_SHADER, "Skydome/Forward/SkydomeShader.fx"}
        }
    },
    { 
        GraphicsApi::ShaderProgramType::TerrainMesh,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Terrain/Mesh/Forward/TerrainMeshShader.fx",},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Terrain/Mesh/Forward/TerrainMeshShader.fx"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::Water,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Water/WaterShader.fx"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Water/WaterShader.fx"}
        }
    },
};
// clang-format on
}  // namespace

GraphicsApi::ShadersFiles GetDxForwardShaderFiles(GraphicsApi::ShaderProgramType shaderType)
{
    return shaderFiles.at(shaderType);
}
bool FileNameExist(GraphicsApi::ShaderProgramType shaderType)
{
    return shaderFiles.count(shaderType) > 0;
}
}  // namespace DirectX
