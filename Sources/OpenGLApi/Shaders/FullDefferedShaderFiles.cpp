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
            {"Game/loadingShader.vert", GraphicsApi::ShaderType::VERTEX_SHADER},
            {"Game/loadingShader.frag", GraphicsApi::ShaderType::FRAGMENT_SHADER}
        }
    },
    {
        GraphicsApi::Shaders::Entity,
        {
            {"Entity/EntityGeometryPassShader.vert", GraphicsApi::ShaderType::VERTEX_SHADER},
            {"Entity/EntityGeometryPassShader.frag", GraphicsApi::ShaderType::FRAGMENT_SHADER}
        }
    },
    {
        GraphicsApi::Shaders::Grass,
        {
            {"Grass/GrassShader.vert", GraphicsApi::ShaderType::VERTEX_SHADER},
            {"Grass/GrassShader.frag", GraphicsApi::ShaderType::FRAGMENT_SHADER},
            {"Grass/GrassShader.geom", GraphicsApi::ShaderType::GEOMETRY_SHADER}
        }
    },
    {
        GraphicsApi::Shaders::Particles,
        {
            {"Particles/ParticlesShader.vert", GraphicsApi::ShaderType::VERTEX_SHADER},
            {"Particles/ParticlesShader.frag", GraphicsApi::ShaderType::FRAGMENT_SHADER}
        }
    },
    {
        GraphicsApi::Shaders::AnimatedParticles,
        {
            {"Particles/AnimatedParticlesShader.vert", GraphicsApi::ShaderType::VERTEX_SHADER},
            {"Particles/AnimatedParticlesShader.frag", GraphicsApi::ShaderType::FRAGMENT_SHADER}
        }
    },
    {
        GraphicsApi::Shaders::Plants,
        {
            {"Terrain/TerrainShader.vert", GraphicsApi::ShaderType::VERTEX_SHADER},
            {"Terrain/TerrainShader.frag", GraphicsApi::ShaderType::FRAGMENT_SHADER},
            {"Terrain/TerrainShader.cs", GraphicsApi::ShaderType::TESS_CONTROL_SHADER},
            {"Terrain/TerrainShader.es", GraphicsApi::ShaderType::TESS_EVALUATION_SHADER}
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
            {"SkyBox/SkyboxVertexShader.vert", GraphicsApi::ShaderType::VERTEX_SHADER},
            {"SkyBox/SkyboxFragmentShader.frag", GraphicsApi::ShaderType::FRAGMENT_SHADER}
        }
    },
    {
        GraphicsApi::Shaders::Terrain,
        {
            {"Terrain/Tessllation/TerrainShader.vert", GraphicsApi::ShaderType::VERTEX_SHADER},
            {"Terrain/Tessllation/TerrainShader.frag", GraphicsApi::ShaderType::FRAGMENT_SHADER},
            {"Terrain/Tessllation/TerrainShader.cs", GraphicsApi::ShaderType::TESS_CONTROL_SHADER},
            {"Terrain/Tessllation/TerrainShader.es", GraphicsApi::ShaderType::TESS_EVALUATION_SHADER}
        }
    },
    {
        GraphicsApi::Shaders::TerrainMesh,
        {
            {"Terrain/Mesh/TerrainShader.vert", GraphicsApi::ShaderType::VERTEX_SHADER},
            {"Terrain/Mesh/TerrainShader.frag", GraphicsApi::ShaderType::FRAGMENT_SHADER}
        }
    },
    {
        GraphicsApi::Shaders::Tree,
        {
            {"Tree/TreeShader.vert", GraphicsApi::ShaderType::VERTEX_SHADER},
            {"Tree/TreeShader.frag", GraphicsApi::ShaderType::FRAGMENT_SHADER}
        }
    },
    {
        GraphicsApi::Shaders::Water,
        {
            {"Terrain/TerrainShader.vert", GraphicsApi::ShaderType::VERTEX_SHADER},
            {"Terrain/TerrainShader.frag", GraphicsApi::ShaderType::FRAGMENT_SHADER},
            {"Terrain/TerrainShader.cs", GraphicsApi::ShaderType::TESS_CONTROL_SHADER},
            {"Terrain/TerrainShader.es", GraphicsApi::ShaderType::TESS_EVALUATION_SHADER}
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

GraphicsApi::ShadersFiles GetFullDefferedShaderFiles(GraphicsApi::Shaders shaderType)
{
    return fullDefferedShaderFiles.at(shaderType);
}
}  // namespace OpenGLApi
