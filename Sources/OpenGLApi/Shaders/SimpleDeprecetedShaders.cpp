#include "SimpleDeprecetedShaders.h"

namespace OpenGLApi
{
namespace
{
// clang-format off
const std::unordered_map<GraphicsApi::ShaderProgramType, GraphicsApi::ShadersFiles> simpleDeprecetedShaders =
{
    {
        GraphicsApi::ShaderProgramType::Loading,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Game/glsl_1.0p/loadingShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Game/glsl_1.0p/loadingShader.frag"}
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
        GraphicsApi::ShaderProgramType::Skydome,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Skydome/Skydome.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Skydome/Skydome.frag"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::Terrain,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Terrain/terrain_VS.glsl"},
            {GraphicsApi::ShaderType::TESS_CONTROL_SHADER, "Terrain/terrain_TC.glsl"},
            {GraphicsApi::ShaderType::TESS_EVALUATION_SHADER,"Terrain/terrain_TE.glsl"},
            {GraphicsApi::ShaderType::GEOMETRY_SHADER, "Terrain/terrain_GS.glsl"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER,"Terrain/terrain_FS.glsl"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::TerrainMesh,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Terrain/Mesh/TerrainShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER,"Terrain/Mesh/TerrainShader.frag"}
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
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Tree/TreeShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Tree/TreeShader.frag"}
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
    }
};
// clang-format on
}  // namespace

GraphicsApi::ShadersFiles GetSimpleDeprecetedShaders(GraphicsApi::ShaderProgramType shaderType)
{
    return simpleDeprecetedShaders.at(shaderType);
}
}  // namespace OpenGLApi
