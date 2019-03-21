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
    GetLocation(heightmap);
    GetLocation(normalmap);
    GetLocation(blendMap);
    GetLocation(backgorundTexture);
    GetLocation(backgorundTextureNormal);
    GetLocation(redTexture);
    GetLocation(redTextureNormal);
    GetLocation(greenTexture);
    GetLocation(greenTextureNormal);
    GetLocation(blueTexture);
    GetLocation(blueTextureNormal);
    GetLocation(rockTexture);
    GetLocation(rockNormalTexture);
    GetLocation(snowTexture);
    GetLocation(snowNormalTexture);
}
void TerrainShader::BindAttributes()
{
}

#define ConnectTerrainTexture(X) Load(TerrainShaderUniforms::X, static_cast<int>(TerrainTextureType::X));

void TerrainShader::ConnectTextureUnits() const
{
    Load(TerrainShaderUniforms::heightmap, (int)TerrainTextureType::heightmap);
    Load(TerrainShaderUniforms::normalmap, (int)TerrainTextureType::normalmap);

    Load(TerrainShaderUniforms::blendMap, (int)TerrainTextureType::blendMap);
    Load(TerrainShaderUniforms::backgorundTexture, (int)TerrainTextureType::backgorundTexture);
    Load(TerrainShaderUniforms::redTexture, (int)TerrainTextureType::redTexture);
    Load(TerrainShaderUniforms::greenTexture, (int)TerrainTextureType::greenTexture);
    Load(TerrainShaderUniforms::blueTexture, (int)TerrainTextureType::blueTexture);
    Load(TerrainShaderUniforms::rockTexture, (int)TerrainTextureType::rockTexture);
    Load(TerrainShaderUniforms::snowTexture, (int)TerrainTextureType::snowTexture);
}

}  // namespace GameEngine
