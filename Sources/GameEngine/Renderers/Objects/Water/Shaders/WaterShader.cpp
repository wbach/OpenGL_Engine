#include "WaterShader.h"

namespace GameEngine
{
WaterShader::WaterShader(GraphicsApi::IGraphicsApi& graphicsApi)
    : ShaderProgram(graphicsApi, GraphicsApi::Shaders::Water)
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
