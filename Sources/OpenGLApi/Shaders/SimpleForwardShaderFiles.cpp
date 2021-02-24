#include "SimpleForwardShaderFiles.h"

namespace OpenGLApi
{
namespace
{
// clang-format off
const std::unordered_map<GraphicsApi::ShaderProgramType, GraphicsApi::ShadersFiles> simpleForwardShaderFiles =
{
    {
        GraphicsApi::ShaderProgramType::Loading,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Game/loadingShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER,"Game/loadingShader.frag"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::Entity,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Entity/Forward/SimpleEntityShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Entity/Forward/SimpleEntityShader.frag"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::InstancesEntity,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Entity/Forward/InstancesSimpleEntityShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Entity/Forward/SimpleEntityShader.frag"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::InstancesSimpleForwadEntity,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Entity/Forward/InstancesSimpleEntityShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Entity/Forward/SimpleEntityShader.frag"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::PreviewEntity,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Entity/PreviewEntity.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Entity/PreviewEntity.frag"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::DebugObject,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "DebugShaders/debugObjectShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "DebugShaders/debugObjectShader.frag"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::Grid,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "DebugShaders/gridShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "DebugShaders/gridShader.frag"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::Line,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "DebugShaders/lineDebugShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "DebugShaders/lineDebugShader.frag"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::Grass,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Grass/Forward/GrassShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Grass/Forward/GrassShader.frag"},
            {GraphicsApi::ShaderType::GEOMETRY_SHADER, "Grass/Forward/GrassShader.geom"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::Particles,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Particles/Forward/ParticlesShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Particles/Forward/ParticlesShader.frag"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::AnimatedParticles,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Particles/Forward/AnimatedParticlesShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Particles/Forward/AnimatedParticlesShader.frag"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::Plants,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Particles/Forward/AnimatedParticlesShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Particles/Forward/AnimatedParticlesShader.frag"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::Shadows,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Shadows/ShadowVertexShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Shadows/ShadowFragmentShader.frag"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::InstancesShadows,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Shadows/InstancedShadowVertexShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Shadows/ShadowFragmentShader.frag"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::SkyBox,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "SkyBox/Forward/SkyboxVertexShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "SkyBox/Forward/SkyboxFragmentShader.frag"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::Skydome,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Skydome/Forward/Skydome.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Skydome/Forward/Skydome.frag"}
        }
    },
    { 
        GraphicsApi::ShaderProgramType::Terrain,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Terrain/Tessllation/Forward/TerrainShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Terrain/Tessllation/Forward/TerrainShader.frag"},
            {GraphicsApi::ShaderType::TESS_CONTROL_SHADER, "Terrain/Tessllation/Forward/TerrainShader.cs"},
            {GraphicsApi::ShaderType::TESS_EVALUATION_SHADER, "Terrain/Tessllation/Forward/TerrainShader.es"}
        }
    },
    { 
        GraphicsApi::ShaderProgramType::TerrainMesh,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Terrain/Mesh/Forward/TerrainShader.vert"},
            {GraphicsApi::ShaderType::GEOMETRY_SHADER, "Terrain/Mesh/Forward/TerrainShader.geom"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Terrain/Mesh/Forward/TerrainShader.frag"}
        }
    },
    { 
        GraphicsApi::ShaderProgramType::Tree,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Tree/Forward/TreeShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Tree/Forward/TreeShader.frag"}
        }
    },
    { 
        GraphicsApi::ShaderProgramType::Water,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Water/simpleWater.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Water/simpleWater.frag"}
        }
    },
    { 
        GraphicsApi::ShaderProgramType::Blur,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "PostProcess/Blur/Blur.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "PostProcess/Blur/Blur.frag"}
        }
    },
    { 
        GraphicsApi::ShaderProgramType::ColorFilper,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "PostProcess/ColorFliper/ColorFliper.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "PostProcess/ColorFliper/ColorFliper.frag"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::Deffered,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Light/LightPassShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Light/LightPassShader.frag"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::SSAO,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "PostProcess/SSAO/SSAOShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "PostProcess/SSAO/SSAOShader.frag"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::Font,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Font/fontVertex.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Font/fontFragment.frag"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::Texture,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Gui/guiVertexShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Gui/guiFragmentShader.frag"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::DebugNormal,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "DebugShaders/NormalShader.vert"},
            {GraphicsApi::ShaderType::GEOMETRY_SHADER, "DebugShaders/NormalShader.geom"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER,"DebugShaders/NormalShader.frag"}
        }
   }
};
// clang-format on
}  // namespace

std::optional<GraphicsApi::ShadersFiles> GetSimpleForwardShaderFiles(GraphicsApi::ShaderProgramType shaderType)
{
    auto filesIter = simpleForwardShaderFiles.find(shaderType);
    if (filesIter != simpleForwardShaderFiles.end())
    {
        return filesIter->second;
    }

    return std::nullopt;
}
}  // namespace OpenGLApi
