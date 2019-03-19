#include "TerrainNormalMapShader.h"
#include "TerrainNormalMapShaderUniforms.h"

#define GetLocation(X) uniforms_[TerrainNormalMapShaderUniforms::X] = GetUniformLocation(#X);

namespace GameEngine
{
TerrainNormalMapShader::TerrainNormalMapShader(GraphicsApi::IGraphicsApi& graphicsApi)
    : ShaderProgram(graphicsApi, GraphicsApi::Shaders::TerrainNormal)
{
}
void TerrainNormalMapShader::GetAllUniformLocations()
{
    uniforms_.resize(TerrainNormalMapShaderUniforms::SIZE);

    GetLocation(normalStrength);
    GetLocation(heightmap);
    GetLocation(N);
}
void TerrainNormalMapShader::BindAttributes()
{
}
void TerrainNormalMapShader::ConnectTextureUnits() const
{
    Load(TerrainNormalMapShaderUniforms::heightmap, 0);
}
}  // namespace GameEngine
