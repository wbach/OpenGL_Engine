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
        GraphicsApi::ShaderProgramType::InstancesEntity,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Entity/InstancesEntityGeometryPassShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Entity/EntityGeometryPassShader.frag"}
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
        GraphicsApi::ShaderProgramType::SimpleForwadEntity,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Entity/Forward/SimpleEntityShader.vert"},
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
        GraphicsApi::ShaderProgramType::InstancesShadows,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Shadows/InstancedShadowVertexShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Shadows/ShadowFragmentShader.frag"}
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
        GraphicsApi::ShaderProgramType::TreeLeafs,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Tree/LeafShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Tree/LeafShader.frag"}
        }
    },
        {
        GraphicsApi::ShaderProgramType::TreeLeafsShdaows,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Tree/LeafShader.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "Tree/Forward/ShadowLeafShader.frag"}
        }
    },
    {
        GraphicsApi::ShaderProgramType::Water,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "Water/waterShader.vert"},
            {GraphicsApi::ShaderType::GEOMETRY_SHADER, "Water/waterShader.geom"},
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
        GraphicsApi::ShaderProgramType::Sky,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "PostProcess/Sky/Sky.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "PostProcess/Sky/Sky.frag"}
        }
    },
        {
        GraphicsApi::ShaderProgramType::ACESFilmicToneMapping,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "PostProcess/ToneMapping/ToneMapping.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "PostProcess/ToneMapping/ToneMapping.frag"}
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
    },
    {
        GraphicsApi::ShaderProgramType::Outline,
        {
            {GraphicsApi::ShaderType::VERTEX_SHADER, "PostProcess/Outline/Outline.vert"},
            {GraphicsApi::ShaderType::FRAGMENT_SHADER, "PostProcess/Outline/Outline.frag"}
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

std::optional<GraphicsApi::ShadersFiles> GetFullDefferedShaderFiles(GraphicsApi::ShaderProgramType shaderType)
{
    auto filesIter = fullDefferedShaderFiles.find(shaderType);
    if (filesIter != fullDefferedShaderFiles.end())
    {
        return filesIter->second;
    }

    return std::nullopt;
}
}  // namespace OpenGLApi
