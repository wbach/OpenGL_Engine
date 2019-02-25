#include "TerrainShader.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainTexturesTypes.h"
#include "TerrainShaderUniforms.h"

#define GetLocation(X) uniforms_[TerrainShaderUniforms::X] = GetUniformLocation(#X);

namespace GameEngine
{
TerrainShader::TerrainShader(GraphicsApi::IGraphicsApi& graphicsApi)
    : ShaderProgram(graphicsApi, GraphicsApi::Shaders::Terrain)
{
}

void TerrainShader::GetAllUniformLocations()
{
    uniforms_.resize(TerrainShaderUniforms::SIZE);

    GetLocation(localMatrix);
    GetLocation(worldMatrix);
    GetLocation(m_ViewProjection);
    GetLocation(index);
    GetLocation(gap);
    GetLocation(lod);
    GetLocation(scaleY);
    GetLocation(location);
    GetLocation(cameraPosition);
    GetLocation(lod_morph_area_1);
    GetLocation(lod_morph_area_2);
    GetLocation(lod_morph_area_3);
    GetLocation(lod_morph_area_4);
    GetLocation(lod_morph_area_5);
    GetLocation(lod_morph_area_6);
    GetLocation(lod_morph_area_7);
    GetLocation(lod_morph_area_8);
}
void TerrainShader::BindAttributes()
{
}

#define ConnectTerrainTexture(X) Load(TerrainShaderUniforms::X, static_cast<int>(TerrainTextureType::X));

void TerrainShader::ConnectTextureUnits() const
{
}

}  // namespace GameEngine
