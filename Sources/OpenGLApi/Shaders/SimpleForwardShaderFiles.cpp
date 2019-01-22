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
            {"Game/loadingShader.vert", GraphicsApi::ShaderType::VERTEX_SHADER},
            {"Game/loadingShader.frag", GraphicsApi::ShaderType::FRAGMENT_SHADER}
        }
    },
    {
        GraphicsApi::Shaders::Entity,
        {
            {"Entity/Forward/SimpleEntityShader.vert", GraphicsApi::ShaderType::VERTEX_SHADER},
            {"Entity/Forward/SimpleEntityShader.frag", GraphicsApi::ShaderType::FRAGMENT_SHADER}
        }
    },
    {
        GraphicsApi::Shaders::Grass,
        {
            {"Grass/Forward/GrassShader.vert", GraphicsApi::ShaderType::VERTEX_SHADER},
            {"Grass/Forward/GrassShader.frag", GraphicsApi::ShaderType::FRAGMENT_SHADER},
            {"Grass/Forward/GrassShader.geom", GraphicsApi::ShaderType::GEOMETRY_SHADER}
        }
    },
    {
        GraphicsApi::Shaders::Particles,
        {
            {"Particles/Forward/ParticlesShader.vert", GraphicsApi::ShaderType::VERTEX_SHADER},
            {"Particles/Forward/ParticlesShader.frag", GraphicsApi::ShaderType::FRAGMENT_SHADER}
        }
    },
    {
        GraphicsApi::Shaders::AnimatedParticles,
        {
            {"Particles/Forward/AnimatedParticlesShader.vert", GraphicsApi::ShaderType::VERTEX_SHADER},
            {"Particles/Forward/AnimatedParticlesShader.frag", GraphicsApi::ShaderType::FRAGMENT_SHADER}
        }
    },
    {
        GraphicsApi::Shaders::Plants,
        {
            {"Terrain/Forward/TerrainShader.vert", GraphicsApi::ShaderType::VERTEX_SHADER},
            {"Terrain/Forward/TerrainShader.frag", GraphicsApi::ShaderType::FRAGMENT_SHADER},
            {"Terrain/Forward/TerrainShader.cs", GraphicsApi::ShaderType::TESS_CONTROL_SHADER},
            {"Terrain/Forward/TerrainShader.es", GraphicsApi::ShaderType::TESS_EVALUATION_SHADER}
        }
    },
    {
        GraphicsApi::Shaders::Shadows,
        {
            {"Shadows/ShadowVertexShader.vert", GraphicsApi::ShaderType::VERTEX_SHADER},
            {"Shadows/ShadowFragmentShader.frag", GraphicsApi::ShaderType::FRAGMENT_SHADER}
        }
    },
    {
        GraphicsApi::Shaders::SkyBox,
        {
            {"SkyBox/Forward/SkyboxVertexShader.vert", GraphicsApi::ShaderType::VERTEX_SHADER},
            {"SkyBox/Forward/SkyboxFragmentShader.frag", GraphicsApi::ShaderType::FRAGMENT_SHADER}
        }
    },
    { 
        GraphicsApi::Shaders::Terrain,
        {
            {"Terrain/Tessllation/Forward/TerrainShader.vert", GraphicsApi::ShaderType::VERTEX_SHADER},
            {"Terrain/Tessllation/Forward/TerrainShader.frag", GraphicsApi::ShaderType::FRAGMENT_SHADER},
            {"Terrain/Tessllation/Forward/TerrainShader.cs", GraphicsApi::ShaderType::TESS_CONTROL_SHADER},
            {"Terrain/Tessllation/Forward/TerrainShader.es", GraphicsApi::ShaderType::TESS_EVALUATION_SHADER}
        }
    },
    { 
        GraphicsApi::Shaders::TerrainMesh,
        {
            {"Terrain/Mesh/Forward/TerrainShader.vert", GraphicsApi::ShaderType::VERTEX_SHADER},
            {"Terrain/Mesh/Forward/TerrainShader.frag", GraphicsApi::ShaderType::FRAGMENT_SHADER}
        }
    },
    { 
        GraphicsApi::Shaders::Tree,
        {
            {"Tree/Forward/TreeShader.vert", GraphicsApi::ShaderType::VERTEX_SHADER},
            {"Tree/Forward/TreeShader.frag", GraphicsApi::ShaderType::FRAGMENT_SHADER}
        }
    },
    { 
        GraphicsApi::Shaders::Water,
        {
            {"Terrain/Forward/TerrainShader.vert", GraphicsApi::ShaderType::VERTEX_SHADER},
            {"Terrain/Forward/TerrainShader.frag", GraphicsApi::ShaderType::FRAGMENT_SHADER},
            {"Terrain/Forward/TerrainShader.cs", GraphicsApi::ShaderType::TESS_CONTROL_SHADER},
            {"Terrain/Forward/TerrainShader.es", GraphicsApi::ShaderType::TESS_EVALUATION_SHADER}
        }
    },
    { 
        GraphicsApi::Shaders::Blur,
        {
            {"PostProcess/Blur/Blur.vert", GraphicsApi::ShaderType::VERTEX_SHADER},
            {"PostProcess/Blur/Blur.frag", GraphicsApi::ShaderType::FRAGMENT_SHADER}
        }
    },
    { 
        GraphicsApi::Shaders::ColorFilper,
        {
            {"PostProcess/ColorFliper/ColorFliper.vert", GraphicsApi::ShaderType::VERTEX_SHADER},
            {"PostProcess/ColorFliper/ColorFliper.frag", GraphicsApi::ShaderType::FRAGMENT_SHADER}
        }
    },
    {
        GraphicsApi::Shaders::Deffered,
        {
            {"Light/LightPassShader.vert", GraphicsApi::ShaderType::VERTEX_SHADER},
            {"Light/LightPassShader.frag", GraphicsApi::ShaderType::FRAGMENT_SHADER}
        }
    },
    {
        GraphicsApi::Shaders::SSAO,
        {
            {"PostProcess/SSAO/SSAOShader.vert", GraphicsApi::ShaderType::VERTEX_SHADER},
            {"PostProcess/SSAO/SSAOShader.frag", GraphicsApi::ShaderType::FRAGMENT_SHADER}
        }
    },
    {
        GraphicsApi::Shaders::Font,
        {
            {"Font/fontVertex.vert", GraphicsApi::ShaderType::VERTEX_SHADER},
            {"Font/fontFragment.frag", GraphicsApi::ShaderType::FRAGMENT_SHADER}
        }
    },
    {
        GraphicsApi::Shaders::Texture,
        {
            {"Gui/guiVertexShader.vert", GraphicsApi::ShaderType::VERTEX_SHADER},
            {"Gui/guiFragmentShader.frag", GraphicsApi::ShaderType::FRAGMENT_SHADER}
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
