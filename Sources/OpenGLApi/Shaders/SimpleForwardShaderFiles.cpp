#include "SimpleForwardShaderFiles.h"

namespace OpenGLApi
{
namespace
{
// clang-format off
const std::unordered_map<GraphicsApi::Shaders, GraphicsApi::ShadersFiles> simpleForwardShaderFiles =
{
    {
        GraphicsApi::Shaders::Loading,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Game/loadingShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER,"Game/loadingShader.frag"}
        }
    },
    {
        GraphicsApi::Shaders::Entity,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Entity/Forward/SimpleEntityShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Entity/Forward/SimpleEntityShader.frag"}
        }
    },
    {
        GraphicsApi::Shaders::Grass,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Grass/Forward/GrassShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Grass/Forward/GrassShader.frag"},
            {GraphicsApi::ShaderType::GEOMETRY_SHADER, "Grass/Forward/GrassShader.geom"}
        }
    },
    {
        GraphicsApi::Shaders::Particles,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Particles/Forward/ParticlesShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Particles/Forward/ParticlesShader.frag"}
        }
    },
    {
        GraphicsApi::Shaders::AnimatedParticles,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Particles/Forward/AnimatedParticlesShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Particles/Forward/AnimatedParticlesShader.frag"}
        }
    },
    {
        GraphicsApi::Shaders::Plants,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Particles/Forward/AnimatedParticlesShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Particles/Forward/AnimatedParticlesShader.frag"}
        }
    },
    {
        GraphicsApi::Shaders::Shadows,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Shadows/ShadowVertexShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Shadows/ShadowFragmentShader.frag"}
        }
    },
    {
        GraphicsApi::Shaders::SkyBox,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "SkyBox/Forward/SkyboxVertexShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "SkyBox/Forward/SkyboxFragmentShader.frag"}
        }
    },
    {
        GraphicsApi::Shaders::Skydome,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Skydome/Forward/Skydome.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Skydome/Forward/Skydome.frag"}
        }
    },
    { 
        GraphicsApi::Shaders::Terrain,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Terrain/Tessllation/Forward/TerrainShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Terrain/Tessllation/Forward/TerrainShader.frag"},
            {GraphicsApi::ShaderType::TESS_CONTROL_SHADER, "Terrain/Tessllation/Forward/TerrainShader.cs"},
            {GraphicsApi::ShaderType::TESS_EVALUATION_SHADER, "Terrain/Tessllation/Forward/TerrainShader.es"}
        }
    },
    { 
        GraphicsApi::Shaders::TerrainMesh,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Terrain/Mesh/Forward/TerrainShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Terrain/Mesh/Forward/TerrainShader.frag"}
        }
    },
    { 
        GraphicsApi::Shaders::Tree,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Tree/Forward/TreeShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Tree/Forward/TreeShader.frag"}
        }
    },
    { 
        GraphicsApi::Shaders::Water,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Tree/Forward/TreeShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Tree/Forward/TreeShader.frag"}
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
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "PostProcess/ColorFliper/ColorFliper.frag"}
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

GraphicsApi::ShadersFiles GetSimpleForwardShaderFiles(GraphicsApi::Shaders shaderType)
{
    return simpleForwardShaderFiles.at(shaderType);
}
}  // namespace OpenGLApi
