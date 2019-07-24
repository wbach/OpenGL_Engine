#include "TerrainMeshShader.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainTexturesTypes.h"
#include "TerrainMeshShaderUniforms.h"

#define GetLocation(X) uniforms_[TerrainMeshShaderUniforms::X] = GetUniformLocation(#X);
#define ConnectTerrainTexture(X) Load(TerrainMeshShaderUniforms::X, static_cast<int>(TerrainTextureType::X));

namespace GameEngine
{
TerrainMeshShader::TerrainMeshShader(GraphicsApi::IGraphicsApi& graphicsApi)
    : ShaderProgram(graphicsApi, GraphicsApi::Shaders::TerrainMesh)
{
}
void TerrainMeshShader::GetAllUniformLocations()
{
    uniforms_.resize(TerrainMeshShaderUniforms::SIZE);

    GetLocation(transformMatrix);
    GetLocation(viewMatrix);
    GetLocation(projectionMatrix);
    GetLocation(toShadowMapSpace);
    GetLocation(useShadows);
    GetLocation(shadowDistance);
    GetLocation(shadowMapSize);
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
}
void TerrainMeshShader::BindAttributes()
{
    BindAttribute(0, "POSITION");
    BindAttribute(1, "TEXTCOORD");
    BindAttribute(2, "NORMAL");
    BindAttribute(3, "TANGENT");
}
void TerrainMeshShader::ConnectTextureUnits() const
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
    //ConnectTerrainTexture(rockTexture);
    //ConnectTerrainTexture(rockNormalTexture);
    //ConnectTerrainTexture(snowTexture);
    //ConnectTerrainTexture(snowNormalTexture);
}
}  // namespace GameEngine
