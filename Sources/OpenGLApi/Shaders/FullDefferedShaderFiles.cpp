#include "FullDefferedShaderFiles.h"

namespace OpenGLApi
{
namespace
{
// clang-format off
const std::unordered_map<GraphicsApi::ShaderProgramType, GraphicsApi::ShadersFiles> fullDefferedShaderFiles =
{
    {
        GraphicsApi::ShaderProgramType::Loading,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Game/loadingShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Game/loadingShader.frag"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::Entity,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Entity/EntityGeometryPassShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Entity/EntityGeometryPassShader.frag"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::SimpleForwadEntity,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Entity/Forward/SimpleEntityShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Entity/Forward/SimpleEntityShader.frag"}
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
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Grass/GrassShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Grass/GrassShader.frag"},
            {GraphicsApi::ShaderType::GEOMETRY_SHADER, "Grass/GrassShader.geom",}
        }
    },
    {
        GraphicsApi::ShaderProgramType::Particles,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Particles/ParticlesShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Particles/ParticlesShader.frag"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::AnimatedParticles,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Particles/AnimatedParticlesShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Particles/AnimatedParticlesShader.frag"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::Plants,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Shadows/ShadowVertexShader.vert", },
            {GraphicsApi::ShaderType::FRAGMENT_SHADER,"Shadows/ShadowFragmentShader.frag"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::Shadows,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Shadows/ShadowVertexShader.vert", },
            {GraphicsApi::ShaderType::FRAGMENT_SHADER,"Shadows/ShadowFragmentShader.frag"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::SkyBox,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "SkyBox/SkyboxVertexShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "SkyBox/SkyboxFragmentShader.frag"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::ForwardSkyBox,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "SkyBox/Forward/SkyboxVertexShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "SkyBox/Forward/SkyboxFragmentShader.frag"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::Skydome,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Skydome/Skydome.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Skydome/Skydome.frag"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::Terrain,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Terrain/Tessllation/TerrainShader.vert"},
            {GraphicsApi::ShaderType::TESS_CONTROL_SHADER, "Terrain/Tessllation/TerrainShader.cs"},
            {GraphicsApi::ShaderType::TESS_EVALUATION_SHADER,"Terrain/Tessllation/TerrainShader.es"},
            {GraphicsApi::ShaderType::GEOMETRY_SHADER, "Terrain/Tessllation/TerrainShader.geom"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER,"Terrain/Tessllation/TerrainShader.frag"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::TerrainMesh,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Terrain/Mesh/TerrainShader.vert"},
            {GraphicsApi::ShaderType::GEOMETRY_SHADER, "Terrain/Mesh/TerrainShader.geom"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER,"Terrain/Mesh/TerrainShader.frag"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::SimpleForwardTerrainMesh,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Terrain/Mesh/Forward/TerrainShader.vert"},
            {GraphicsApi::ShaderType::GEOMETRY_SHADER, "Terrain/Mesh/Forward/TerrainShader.geom"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Terrain/Mesh/Forward/TerrainShader.frag"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::Tree,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Tree/TreeShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Tree/TreeShader.frag"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::Water,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Water/waterShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Water/waterShader.frag"}
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
            {GraphicsApi::ShaderType::FRAGMENT_SHADER,"PostProcess/ColorFliper/ColorFliper.frag"}
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
        GraphicsApi::ShaderProgramType::Fxaa,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "PostProcess/Fxaa/Fxaa.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "PostProcess/Fxaa/Fxaa.frag"}
        }
    }
};
// clang-format on
}  // namespace

GraphicsApi::ShadersFiles GetFullDefferedShaderFiles(GraphicsApi::ShaderProgramType shaderType)
{
    return fullDefferedShaderFiles.at(shaderType);
}
}  // namespace OpenGLApi
