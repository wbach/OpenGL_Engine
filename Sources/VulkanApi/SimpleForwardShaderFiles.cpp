#include "SimpleForwardShaderFiles.h"

#include <optional>
#include <unordered_map>
#include <vector>

#include "PipelineConfig.h"
#include "ShaderFiles.h"

namespace GraphicsApi::Vulkan
{
namespace
{

const uint32 PER_APP_BIND_LOCATION              = 0;
const uint32 PER_FRAME_BIND_LOCATION            = 1;
const uint32 PER_OBJECT_CONSTANTS_BIND_LOCATION = 2;
const uint32 PER_OBJECT_UPDATE_BIND_LOCATION    = 3;  // PER_TERRAIN_BIND_LOCATION
const uint32 PER_POSE_UPDATE_BIND_LOCATION      = 4;  // PER_NODE_LOCATION
const uint32 PER_INSTANCES_BIND_LOCATION        = 5;
const uint32 PER_MESH_OBJECT_BIND_LOCATION      = 6;
const uint32 SHADOW_BUFFER_BIND_LOCATION        = 7;
const uint32 WATER_TILE_BIND_LOCATION           = 8;
const uint32 DIRECTIONAL_LIGHTS_BIND_LOCATION   = 9;
const uint32 POINT_LIGHTS_BIND_LOCATION         = 10;
const uint32 SPOT_LIGHTS_BIND_LOCATION          = 11;

const std::unordered_map<GraphicsApi::ShaderProgramType, PipelineConfig> pipelineConfigurations = {
    {GraphicsApi::ShaderProgramType::Loading,
     PipelineConfig{
         .shaders                = ShadersFiles{{GraphicsApi::ShaderType::VERTEX_SHADER, "Common/Quad.vert"},
                                 {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Game/loadingShader.frag"}},
         .topology               = Topology::TriangleList,
         .polygonMode            = PolygonMode::Fill,
         .cullMode               = CullMode::None,
         .depthTestEnable        = false,
         .depthWriteEnable       = false,
         .blendEnable            = false,
         .primitiveRestartEnable = false,
         .vertexLayout           = {},
         .descriptorBindings     = {}
     }},
    {GraphicsApi::ShaderProgramType::Entity,
     PipelineConfig{
         .shaders                = ShadersFiles{{GraphicsApi::ShaderType::VERTEX_SHADER, "Entity/Forward/SimpleEntityShader.vert"},
                                 {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Entity/Forward/SimpleEntityShader.frag"}},
         .topology               = Topology::TriangleList,
         .polygonMode            = PolygonMode::Fill,
         .cullMode               = CullMode::Back,
         .depthTestEnable        = true,
         .depthWriteEnable       = true,
         .blendEnable            = false,
         .primitiveRestartEnable = false,
         .vertexLayout           = GetDefaultVertexLayout(),
         .descriptorBindings =
             {// 0 - PER_APP
              {.binding         = PER_APP_BIND_LOCATION,
               .descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
               .descriptorCount = 1,
               .stageFlags      = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT},

              // 1 - PER_FRAME
              {.binding         = PER_FRAME_BIND_LOCATION,
               .descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
               .descriptorCount = 1,
               .stageFlags      = VK_SHADER_STAGE_VERTEX_BIT},

              // 2 - PER_OBJECT_CONSTANTS
              {.binding         = PER_OBJECT_CONSTANTS_BIND_LOCATION,
               .descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
               .descriptorCount = 1,
               .stageFlags      = VK_SHADER_STAGE_VERTEX_BIT},

              // 3 - PER_OBJECT_UPDATE
              {.binding         = PER_OBJECT_UPDATE_BIND_LOCATION,
               .descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
               .descriptorCount = 1,
               .stageFlags      = VK_SHADER_STAGE_VERTEX_BIT},

              // 4 - PER_POSE_UPDATE
              {.binding         = PER_POSE_UPDATE_BIND_LOCATION,
               .descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
               .descriptorCount = 1,
               .stageFlags      = VK_SHADER_STAGE_VERTEX_BIT},

              {.binding         = PER_MESH_OBJECT_BIND_LOCATION,  // perMaterial
               .descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
               .descriptorCount = 1,
               .stageFlags      = VK_SHADER_STAGE_FRAGMENT_BIT}}

     }},
    {GraphicsApi::ShaderProgramType::InstancesEntity,
     PipelineConfig{
         .shaders                = ShadersFiles{{GraphicsApi::ShaderType::VERTEX_SHADER, "Entity/Forward/InstancesSimpleEntityShader.vert"},
                                 {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Entity/Forward/SimpleEntityShader.frag"}},
         .topology               = Topology::TriangleList,
         .polygonMode            = PolygonMode::Fill,
         .cullMode               = CullMode::Back,
         .depthTestEnable        = true,
         .depthWriteEnable       = true,
         .blendEnable            = false,
         .primitiveRestartEnable = false,
         .vertexLayout           = {},
         .descriptorBindings     = {}
     }},
    {GraphicsApi::ShaderProgramType::InstancesSimpleForwadEntity,
     PipelineConfig{
         .shaders                = ShadersFiles{{GraphicsApi::ShaderType::VERTEX_SHADER, "Entity/Forward/InstancesSimpleEntityShader.vert"},
                                 {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Entity/Forward/SimpleEntityShader.frag"}},
         .topology               = Topology::TriangleList,
         .polygonMode            = PolygonMode::Fill,
         .cullMode               = CullMode::Back,
         .depthTestEnable        = true,
         .depthWriteEnable       = true,
         .blendEnable            = false,
         .primitiveRestartEnable = false,
         .vertexLayout           = {},
         .descriptorBindings     = {}
     }},
    {GraphicsApi::ShaderProgramType::PreviewEntity,
     PipelineConfig{
         .shaders                = ShadersFiles{{GraphicsApi::ShaderType::VERTEX_SHADER, "Entity/PreviewEntity.vert"},
                                 {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Entity/PreviewEntity.frag"}},
         .topology               = Topology::TriangleList,
         .polygonMode            = PolygonMode::Fill,
         .cullMode               = CullMode::Back,
         .depthTestEnable        = true,
         .depthWriteEnable       = true,
         .blendEnable            = false,
         .primitiveRestartEnable = false,
         .vertexLayout           = {},
         .descriptorBindings     = {}
     }},
    {GraphicsApi::ShaderProgramType::DebugObject,
     PipelineConfig{
         .shaders                = ShadersFiles{{GraphicsApi::ShaderType::VERTEX_SHADER, "DebugShaders/debugObjectShader.vert"},
                                 {GraphicsApi::ShaderType::FRAGMENT_SHADER, "DebugShaders/debugObjectShader.frag"}},
         .topology               = Topology::TriangleList,
         .polygonMode            = PolygonMode::Fill,
         .cullMode               = CullMode::Back,
         .depthTestEnable        = true,
         .depthWriteEnable       = true,
         .blendEnable            = false,
         .primitiveRestartEnable = false,
         .vertexLayout           = {},
         .descriptorBindings     = {}
     }},
    {GraphicsApi::ShaderProgramType::Grid,
     PipelineConfig{
         .shaders                = ShadersFiles{{GraphicsApi::ShaderType::VERTEX_SHADER, "DebugShaders/gridShader.vert"},
                                 {GraphicsApi::ShaderType::FRAGMENT_SHADER, "DebugShaders/gridShader.frag"}},
         .topology               = Topology::TriangleList,
         .polygonMode            = PolygonMode::Fill,
         .cullMode               = CullMode::None,
         .depthTestEnable        = true,
         .depthWriteEnable       = true,
         .blendEnable            = true,
         .primitiveRestartEnable = false,
         .vertexLayout           = {},
         .descriptorBindings     = {}
     }},
    {GraphicsApi::ShaderProgramType::Line,
     PipelineConfig{
         .shaders                = ShadersFiles{{GraphicsApi::ShaderType::VERTEX_SHADER, "DebugShaders/lineDebugShader.vert"},
                                 {GraphicsApi::ShaderType::FRAGMENT_SHADER, "DebugShaders/lineDebugShader.frag"}},
         .topology               = Topology::LineList,
         .polygonMode            = PolygonMode::Line,
         .cullMode               = CullMode::None,
         .depthTestEnable        = true,
         .depthWriteEnable       = false,
         .blendEnable            = false,
         .primitiveRestartEnable = false,
         .vertexLayout           = {},
         .descriptorBindings     = {}
     }},
    {GraphicsApi::ShaderProgramType::Grass,
     PipelineConfig{
         .shaders                = ShadersFiles{{GraphicsApi::ShaderType::VERTEX_SHADER, "Grass/Forward/GrassShader.vert"},
                                 {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Grass/Forward/GrassShader.frag"}},
         .topology               = Topology::TriangleList,
         .polygonMode            = PolygonMode::Fill,
         .cullMode               = CullMode::None,
         .depthTestEnable        = true,
         .depthWriteEnable       = true,
         .blendEnable            = false,
         .primitiveRestartEnable = false,
         .vertexLayout           = {},
         .descriptorBindings     = {}
     }},
    {GraphicsApi::ShaderProgramType::TreeLeafs,
     PipelineConfig{
         .shaders                = ShadersFiles{{GraphicsApi::ShaderType::VERTEX_SHADER, "Tree/LeafShader.vert"},
                                 {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Tree/Forward/LeafShader.frag"}},
         .topology               = Topology::TriangleList,
         .polygonMode            = PolygonMode::Fill,
         .cullMode               = CullMode::None,
         .depthTestEnable        = true,
         .depthWriteEnable       = true,
         .blendEnable            = false,
         .primitiveRestartEnable = false,
         .vertexLayout           = {},
         .descriptorBindings     = {}
     }},
    {GraphicsApi::ShaderProgramType::TreeLeafsCluster,
     PipelineConfig{
         .shaders                = ShadersFiles{{GraphicsApi::ShaderType::VERTEX_SHADER, "Tree/LeafClusterShader.vert"},
                                 {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Tree/Forward/LeafClusterShader.frag"}},
         .topology               = Topology::TriangleList,
         .polygonMode            = PolygonMode::Fill,
         .cullMode               = CullMode::None,
         .depthTestEnable        = true,
         .depthWriteEnable       = true,
         .blendEnable            = false,
         .primitiveRestartEnable = false,
         .vertexLayout           = {},
         .descriptorBindings     = {}
     }},
    {GraphicsApi::ShaderProgramType::TreeLeafsClusterCreation,
     PipelineConfig{
         .shaders                = ShadersFiles{{GraphicsApi::ShaderType::VERTEX_SHADER, "Tree/LeafClusterCreationShader.vert"},
                                 {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Tree/LeafClusterCreationShader.frag"}},
         .topology               = Topology::TriangleList,
         .polygonMode            = PolygonMode::Fill,
         .cullMode               = CullMode::None,
         .depthTestEnable        = true,
         .depthWriteEnable       = true,
         .blendEnable            = false,
         .primitiveRestartEnable = false,
         .vertexLayout           = {},
         .descriptorBindings     = {}
     }},
    {GraphicsApi::ShaderProgramType::Particles,
     PipelineConfig{
         .shaders                = ShadersFiles{{GraphicsApi::ShaderType::VERTEX_SHADER, "Particles/Forward/ParticlesShader.vert"},
                                 {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Particles/Forward/ParticlesShader.frag"}},
         .topology               = Topology::TriangleList,
         .polygonMode            = PolygonMode::Fill,
         .cullMode               = CullMode::None,
         .depthTestEnable        = true,
         .depthWriteEnable       = false,
         .blendEnable            = true,
         .primitiveRestartEnable = false,
         .vertexLayout           = {},
         .descriptorBindings     = {}
     }},
    {GraphicsApi::ShaderProgramType::AnimatedParticles,
     PipelineConfig{
         .shaders                = ShadersFiles{{GraphicsApi::ShaderType::VERTEX_SHADER, "Particles/Forward/AnimatedParticlesShader.vert"},
                                 {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Particles/Forward/AnimatedParticlesShader.frag"}},
         .topology               = Topology::TriangleList,
         .polygonMode            = PolygonMode::Fill,
         .cullMode               = CullMode::None,
         .depthTestEnable        = true,
         .depthWriteEnable       = false,
         .blendEnable            = true,
         .primitiveRestartEnable = false,
         .vertexLayout           = {},
         .descriptorBindings     = {}
     }},
    {GraphicsApi::ShaderProgramType::Plants,
     PipelineConfig{
         .shaders                = ShadersFiles{{GraphicsApi::ShaderType::VERTEX_SHADER, "Particles/Forward/AnimatedParticlesShader.vert"},
                                 {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Particles/Forward/AnimatedParticlesShader.frag"}},
         .topology               = Topology::TriangleList,
         .polygonMode            = PolygonMode::Fill,
         .cullMode               = CullMode::None,
         .depthTestEnable        = true,
         .depthWriteEnable       = true,
         .blendEnable            = false,
         .primitiveRestartEnable = false,
         .vertexLayout           = {},
         .descriptorBindings     = {}
     }},
    {GraphicsApi::ShaderProgramType::Shadows,
     PipelineConfig{
         .shaders                = ShadersFiles{{GraphicsApi::ShaderType::VERTEX_SHADER, "Shadows/ShadowVertexShader.vert"},
                                 {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Shadows/ShadowFragmentShader.frag"}},
         .topology               = Topology::TriangleList,
         .polygonMode            = PolygonMode::Fill,
         .cullMode               = CullMode::Back,
         .depthTestEnable        = true,
         .depthWriteEnable       = true,
         .blendEnable            = false,
         .primitiveRestartEnable = false,
         .vertexLayout           = {},
         .descriptorBindings     = {}
     }},
    {GraphicsApi::ShaderProgramType::InstancesShadows,
     PipelineConfig{
         .shaders                = ShadersFiles{{GraphicsApi::ShaderType::VERTEX_SHADER, "Shadows/InstancedShadowVertexShader.vert"},
                                 {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Shadows/ShadowFragmentShader.frag"}},
         .topology               = Topology::TriangleList,
         .polygonMode            = PolygonMode::Fill,
         .cullMode               = CullMode::Back,
         .depthTestEnable        = true,
         .depthWriteEnable       = true,
         .blendEnable            = false,
         .primitiveRestartEnable = false,
         .vertexLayout           = {},
         .descriptorBindings     = {}
     }},
    {GraphicsApi::ShaderProgramType::SkyBox,
     PipelineConfig{
         .shaders                = ShadersFiles{{GraphicsApi::ShaderType::VERTEX_SHADER, "SkyBox/Forward/SkyboxVertexShader.vert"},
                                 {GraphicsApi::ShaderType::FRAGMENT_SHADER, "SkyBox/Forward/SkyboxFragmentShader.frag"}},
         .topology               = Topology::TriangleList,
         .polygonMode            = PolygonMode::Fill,
         .cullMode               = CullMode::None,
         .depthTestEnable        = true,
         .depthWriteEnable       = false,
         .blendEnable            = false,
         .primitiveRestartEnable = false,
         .vertexLayout           = {},
         .descriptorBindings     = {}
     }},
    {GraphicsApi::ShaderProgramType::Skydome,
     PipelineConfig{
         .shaders                = ShadersFiles{{GraphicsApi::ShaderType::VERTEX_SHADER, "Skydome/Forward/Skydome.vert"},
                                 {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Skydome/Forward/Skydome.frag"}},
         .topology               = Topology::TriangleList,
         .polygonMode            = PolygonMode::Fill,
         .cullMode               = CullMode::None,
         .depthTestEnable        = true,
         .depthWriteEnable       = false,
         .blendEnable            = false,
         .primitiveRestartEnable = false,
         .vertexLayout           = {},
         .descriptorBindings     = {}
     }},
    {GraphicsApi::ShaderProgramType::SimpleSky,
     PipelineConfig{
         .shaders                = ShadersFiles{{GraphicsApi::ShaderType::VERTEX_SHADER, "PostProcess/Sky/SimpleSky.vert"},
                                 {GraphicsApi::ShaderType::FRAGMENT_SHADER, "PostProcess/Sky/SimpleSky.frag"}},
         .topology               = Topology::TriangleList,
         .polygonMode            = PolygonMode::Fill,
         .cullMode               = CullMode::None,
         .depthTestEnable        = true,
         .depthWriteEnable       = false,
         .blendEnable            = false,
         .primitiveRestartEnable = false,
         .vertexLayout           = {},
         .descriptorBindings     = {}
     }},
    {GraphicsApi::ShaderProgramType::Terrain,
     PipelineConfig{
         .shaders                = ShadersFiles{{GraphicsApi::ShaderType::VERTEX_SHADER, "Terrain/Tessllation/Forward/TerrainShader.vert"},
                                 {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Terrain/Tessllation/Forward/TerrainShader.frag"},
                                 {GraphicsApi::ShaderType::TESS_CONTROL_SHADER, "Terrain/Tessllation/Forward/TerrainShader.cs"},
                                 {GraphicsApi::ShaderType::TESS_EVALUATION_SHADER, "Terrain/Tessllation/Forward/TerrainShader.es"}},
         .topology               = Topology::PatchList,
         .polygonMode            = PolygonMode::Fill,
         .cullMode               = CullMode::Back,
         .depthTestEnable        = true,
         .depthWriteEnable       = true,
         .blendEnable            = false,
         .primitiveRestartEnable = false,
         .vertexLayout           = {},
         .descriptorBindings     = {}
     }},
    {GraphicsApi::ShaderProgramType::TerrainMesh,
     PipelineConfig{
         .shaders                = ShadersFiles{{GraphicsApi::ShaderType::VERTEX_SHADER, "Terrain/Mesh/Forward/TerrainShader.vert"},
                                 {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Terrain/Mesh/Forward/TerrainShader.frag"}},
         .topology               = Topology::TriangleStrip,
         .polygonMode            = PolygonMode::Fill,
         .cullMode               = CullMode::Back,
         .depthTestEnable        = true,
         .depthWriteEnable       = true,
         .blendEnable            = false,
         .primitiveRestartEnable = false,
         .vertexLayout           = GetDefaultVertexLayout(),
         .descriptorBindings =
             {// 0 - PER_APP
              {.binding         = PER_APP_BIND_LOCATION,
               .descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
               .descriptorCount = 1,
               .stageFlags      = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT},

              // 1 - PER_FRAME
              {.binding         = PER_FRAME_BIND_LOCATION,
               .descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
               .descriptorCount = 1,
               .stageFlags      = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT},

              // 3 - PER_OBJECT_UPDATE
              {.binding         = PER_OBJECT_UPDATE_BIND_LOCATION,
               .descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
               .descriptorCount = 1,
               .stageFlags      = VK_SHADER_STAGE_VERTEX_BIT},

              // 6 - PER_MESH_OBJECT - PerTerrainTexturesBuffer
              {.binding         = PER_MESH_OBJECT_BIND_LOCATION,
               .descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
               .descriptorCount = 1,
               .stageFlags      = VK_SHADER_STAGE_FRAGMENT_BIT}}
     }},
    {GraphicsApi::ShaderProgramType::Water,
     PipelineConfig{
         .shaders                = ShadersFiles{{GraphicsApi::ShaderType::VERTEX_SHADER, "Water/simpleWater.vert"},
                                 {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Water/simpleWater.frag"}},
         .topology               = Topology::TriangleStrip,
         .polygonMode            = PolygonMode::Fill,
         .cullMode               = CullMode::Back,
         .depthTestEnable        = true,
         .depthWriteEnable       = false,
         .blendEnable            = true,
         .primitiveRestartEnable = false,
         .vertexLayout           = {},
         .descriptorBindings     = {}
     }},
    {GraphicsApi::ShaderProgramType::Texture,
     PipelineConfig{
         .shaders                = ShadersFiles{{GraphicsApi::ShaderType::VERTEX_SHADER, "Common/Quad.vert"},
                                 {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Gui/guiFragmentShader.frag"}},
         .topology               = Topology::TriangleList,
         .polygonMode            = PolygonMode::Fill,
         .cullMode               = CullMode::None,
         .depthTestEnable        = false,
         .depthWriteEnable       = false,
         .blendEnable            = true,
         .primitiveRestartEnable = false,
         .vertexLayout           = {},
         .descriptorBindings     = {}
     }},
    {GraphicsApi::ShaderProgramType::DebugNormal,
     PipelineConfig{
         .shaders                = ShadersFiles{{GraphicsApi::ShaderType::VERTEX_SHADER, "DebugShaders/NormalShader.vert"},
                                 {GraphicsApi::ShaderType::GEOMETRY_SHADER, "DebugShaders/NormalShader.geom"},
                                 {GraphicsApi::ShaderType::FRAGMENT_SHADER, "DebugShaders/NormalShader.frag"}},
         .topology               = Topology::TriangleList,
         .polygonMode            = PolygonMode::Fill,
         .cullMode               = CullMode::Back,
         .depthTestEnable        = true,
         .depthWriteEnable       = true,
         .blendEnable            = false,
         .primitiveRestartEnable = false,
         .vertexLayout           = {},
         .descriptorBindings     = {}
     }}
};

}  // namespace

std::optional<PipelineConfig> GetSimpleForwardShaderFiles(GraphicsApi::ShaderProgramType shaderType)
{
    auto configIter = pipelineConfigurations.find(shaderType);
    if (configIter != pipelineConfigurations.end())
    {
        return configIter->second;
    }

    return std::nullopt;
}

}  // namespace GraphicsApi::Vulkan