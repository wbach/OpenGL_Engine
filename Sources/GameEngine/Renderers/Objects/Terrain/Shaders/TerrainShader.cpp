#include "TerrainShader.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainTexturesTypes.h"
#include "TerrainShaderUniforms.h"

#define GetLocation(X) uniforms_[static_cast<int>(TerrainShaderUniforms::X)] = GetUniformLocation(#X);
#define ConnectTerrainTexture(X) \
    Load(static_cast<int>(TerrainShaderUniforms::X), static_cast<int>(TerrainTextureType::X));

namespace GameEngine
{
TerrainShader::TerrainShader(GraphicsApi::IGraphicsApi& graphicsApi)
    : ShaderProgram(graphicsApi, GraphicsApi::Shaders::Terrain)
{
}

void TerrainShader::GetAllUniformLocations()
{
    uniforms_.resize(static_cast<int>(TerrainShaderUniforms::SIZE));

    GetLocation(heightmap);
    GetLocation(normalmap);
    GetLocation(blendMap);
    GetLocation(backgorundTexture);
    GetLocation(backgorundTextureNormal);
    GetLocation(backgorundTextureDisplacement);
    GetLocation(redTexture);
    GetLocation(redTextureNormal);
    GetLocation(redTextureDisplacement);
    GetLocation(greenTexture);
    GetLocation(greenTextureNormal);
    GetLocation(greenTextureDisplacement);
    GetLocation(blueTexture);
    GetLocation(blueTextureNormal);
    GetLocation(blueTextureDisplacement);
}

void TerrainShader::ConnectTextureUnits() const
{
    ConnectTerrainTexture(blendMap);
    ConnectTerrainTexture(normalmap);
    ConnectTerrainTexture(heightmap);
    ConnectTerrainTexture(backgorundTexture);
    ConnectTerrainTexture(backgorundTextureNormal);
    ConnectTerrainTexture(backgorundTextureDisplacement);
    ConnectTerrainTexture(redTexture);
    ConnectTerrainTexture(redTextureNormal);
    ConnectTerrainTexture(redTextureDisplacement);
    ConnectTerrainTexture(greenTexture);
    ConnectTerrainTexture(greenTextureNormal);
    ConnectTerrainTexture(greenTextureDisplacement);
    ConnectTerrainTexture(blueTexture);
    ConnectTerrainTexture(blueTextureNormal);
    ConnectTerrainTexture(blueTextureDisplacement);
}

void TerrainShader::BindAttributes()
{
}

}  // namespace GameEngine
