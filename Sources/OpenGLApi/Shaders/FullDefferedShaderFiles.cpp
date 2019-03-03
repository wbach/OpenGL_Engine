#include "FullDefferedShaderFiles.h"

namespace OpenGLApi
{
namespace
{
// clang-format off
const std::unordered_map<GraphicsApi::Shaders, GraphicsApi::ShadersFiles> fullDefferedShaderFiles =
{
    {
        GraphicsApi::Shaders::Loading,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Game/loadingShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Game/loadingShader.frag"}
        }
    },
    {
        GraphicsApi::Shaders::Entity,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Entity/EntityGeometryPassShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Entity/EntityGeometryPassShader.frag"}
        }
    },
    {
        GraphicsApi::Shaders::Grass,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Grass/GrassShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Grass/GrassShader.frag"},
            {GraphicsApi::ShaderType::GEOMETRY_SHADER, "Grass/GrassShader.geom",}
        }
    },
    {
        GraphicsApi::Shaders::Particles,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Particles/ParticlesShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Particles/ParticlesShader.frag"}
        }
    },
    {
        GraphicsApi::Shaders::AnimatedParticles,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Particles/AnimatedParticlesShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Particles/AnimatedParticlesShader.frag"}
        }
    },
    {
        GraphicsApi::Shaders::Plants,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Shadows/ShadowVertexShader.vert", },
            {GraphicsApi::ShaderType::FRAGMENT_SHADER,"Shadows/ShadowFragmentShader.frag"}
        }
    },
    {
        GraphicsApi::Shaders::Shadows,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Shadows/ShadowVertexShader.vert", },
            {GraphicsApi::ShaderType::FRAGMENT_SHADER,"Shadows/ShadowFragmentShader.frag"}
        }
    },
    {
        GraphicsApi::Shaders::SkyBox,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "SkyBox/SkyboxVertexShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "SkyBox/SkyboxFragmentShader.frag"}
        }
    },
    {
        GraphicsApi::Shaders::Skydome,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Skydome/Skydome.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Skydome/Skydome.frag"}
        }
    },
    {
        GraphicsApi::Shaders::Terrain,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Terrain/terrain_VS.glsl"},
            {GraphicsApi::ShaderType::TESS_CONTROL_SHADER, "Terrain/terrain_TC.glsl"},
            {GraphicsApi::ShaderType::TESS_EVALUATION_SHADER,"Terrain/terrain_TE.glsl"},
            {GraphicsApi::ShaderType::GEOMETRY_SHADER, "Terrain/terrain_GS.glsl"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER,"Terrain/terrain_FS.glsl"}
        }
    },
    {
        GraphicsApi::Shaders::TerrainMesh,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Terrain/Mesh/TerrainShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER,"Terrain/Mesh/TerrainShader.frag"}
        }
    },
    {
        GraphicsApi::Shaders::Tree,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Tree/TreeShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Tree/TreeShader.frag"}
        }
    },
    {
        GraphicsApi::Shaders::Water,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Tree/TreeShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Tree/TreeShader.frag"}
        }
    },
    {
        GraphicsApi::Shaders::Blur,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "PostProcess/Blur/Blur.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "PostProcess/Blur/Blur.frag"}
        }
    },
    {
        GraphicsApi::Shaders::ColorFilper,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "PostProcess/ColorFliper/ColorFliper.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER,"PostProcess/ColorFliper/ColorFliper.frag"}
        }
    },
    {
        GraphicsApi::Shaders::Deffered,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Light/LightPassShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Light/LightPassShader.frag"}
        }
    },
    {
        GraphicsApi::Shaders::SSAO,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "PostProcess/SSAO/SSAOShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "PostProcess/SSAO/SSAOShader.frag"}
        }
    },
    {
        GraphicsApi::Shaders::Font,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Font/fontVertex.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Font/fontFragment.frag"}
        }
    },
    {
        GraphicsApi::Shaders::Texture,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Gui/guiVertexShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Gui/guiFragmentShader.frag"}
        }
    }
};
// clang-format on
}  // namespace

GraphicsApi::ShadersFiles GetFullDefferedShaderFiles(GraphicsApi::Shaders shaderType)
{
    return fullDefferedShaderFiles.at(shaderType);
}
}  // namespace OpenGLApi
