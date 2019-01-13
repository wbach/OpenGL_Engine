#include "WaterShader.h"

namespace GameEngine
{
WaterShader::WaterShader(IGraphicsApi& graphicsApi)
    : ShaderProgram(graphicsApi, Shaders::Water)
{
}

void WaterShader::GetAllUniformLocations()
{
}
void WaterShader::BindAttributes()
{
}

void WaterShader::ConnectTextureUnits() const
{
    // ConnectTerrainTexture(blendMap);
}
}  // namespace GameEngine
