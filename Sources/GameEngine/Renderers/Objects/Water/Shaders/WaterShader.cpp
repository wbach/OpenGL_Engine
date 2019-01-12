#include "WaterShader.h"

namespace GameEngine
{
WaterShader::WaterShader(IGraphicsApiPtr graphicsApi)
    : ShaderProgram(graphicsApi, graphicsApi->GetShaderFiles(Shaders::Water))
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
}
