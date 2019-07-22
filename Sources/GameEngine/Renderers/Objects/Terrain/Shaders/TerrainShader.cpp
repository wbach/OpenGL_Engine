#include "TerrainShader.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainTexturesTypes.h"
#include "TerrainShaderUniforms.h"

#define GetLocation(X) uniforms_[static_cast<int>(TerrainShaderUniforms::X)] = GetUniformLocation(#X);
#define ConnectTerrainTexture(X) Load(TerrainShaderUniforms::X, static_cast<int>(TerrainTextureType::X));

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

void TerrainShader::ConnectTextureUnits() const
{
    Load((int)TerrainShaderUniforms::heightmap, (int)TerrainTextureType::heightmap);
    Load((int)TerrainShaderUniforms::normalmap, (int)TerrainTextureType::normalmap);

    Load((int)TerrainShaderUniforms::blendMap, (int)TerrainTextureType::blendMap);
    Load((int)TerrainShaderUniforms::backgorundTexture, (int)TerrainTextureType::backgorundTexture);
    Load((int)TerrainShaderUniforms::backgorundTextureNormal, (int)TerrainTextureType::backgorundTextureNormal);
    Load((int)TerrainShaderUniforms::redTexture, (int)TerrainTextureType::redTexture);
    Load((int)TerrainShaderUniforms::redTextureNormal, (int)TerrainTextureType::redTextureNormal);
    Load((int)TerrainShaderUniforms::greenTexture, (int)TerrainTextureType::greenTexture);
    Load((int)TerrainShaderUniforms::greenTextureNormal, (int)TerrainTextureType::greenTextureNormal);
    Load((int)TerrainShaderUniforms::blueTexture, (int)TerrainTextureType::blueTexture);
    Load((int)TerrainShaderUniforms::blueTextureNormal, (int)TerrainTextureType::blueTextureNormal);
    Load((int)TerrainShaderUniforms::rockTexture, (int)TerrainTextureType::rockTexture);
    Load((int)TerrainShaderUniforms::rockNormalTexture, (int)TerrainTextureType::rockNormalTexture);
    Load((int)TerrainShaderUniforms::snowTexture, (int)TerrainTextureType::snowTexture);
    Load((int)TerrainShaderUniforms::snowNormalTexture, (int)TerrainTextureType::snowNormalTexture);
}

}  // namespace GameEngine
