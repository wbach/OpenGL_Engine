#include "TerrainShader.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainTexturesTypes.h"
#include "TerrainShaderUniforms.h"

#define GetLocation(X) uniforms_[TerrainShaderUniforms::X] = GetUniformLocation(#X);

namespace GameEngine
{
TerrainShader::TerrainShader(IGraphicsApi& graphicsApi)
    : ShaderProgram(graphicsApi, Shaders::Terrain)
{
}

void TerrainShader::GetAllUniformLocations()
{
    uniforms_.resize(TerrainShaderUniforms::SIZE);

    GetLocation(playerPosition);
    GetLocation(lightDirection);
    GetLocation(modelViewProjectionMatrix);
    GetLocation(displacementTexture);
    GetLocation(modelViewMatrix);
    GetLocation(projectionMatrix);
    GetLocation(heightFactor);
    GetLocation(toShadowMapSpace);
    GetLocation(shadowVariables);
    GetLocation(shadowMap);
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
    GetLocation(displacementMap);
}
void TerrainShader::BindAttributes()
{
}

#define ConnectTerrainTexture(X) Load(TerrainShaderUniforms::X, static_cast<int>(TerrainTextureType::X));

void TerrainShader::ConnectTextureUnits() const
{
    ConnectTerrainTexture(blendMap);
    ConnectTerrainTexture(backgorundTexture);
    ConnectTerrainTexture(backgorundTextureNormal);
    ConnectTerrainTexture(redTexture);
    ConnectTerrainTexture(redTextureNormal);
    ConnectTerrainTexture(greenTexture);
    ConnectTerrainTexture(greenTextureNormal);
    ConnectTerrainTexture(blueTexture);
    ConnectTerrainTexture(blueTextureNormal);
    ConnectTerrainTexture(rockTexture);
    ConnectTerrainTexture(rockNormalTexture);
    ConnectTerrainTexture(snowTexture);
    ConnectTerrainTexture(snowNormalTexture);
    ConnectTerrainTexture(displacementMap);
}

}  // namespace GameEngine
