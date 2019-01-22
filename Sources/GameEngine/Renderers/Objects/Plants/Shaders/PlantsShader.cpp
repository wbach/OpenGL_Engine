#include "PlantsShader.h"

namespace GameEngine
{
PlantsShader::PlantsShader(GraphicsApi::IGraphicsApi& graphicsApi)
    : ShaderProgram(graphicsApi, GraphicsApi::Shaders::Plants)
{

}

void PlantsShader::GetAllUniformLocations()
{

}

void PlantsShader::BindAttributes()
{
}

void PlantsShader::ConnectTextureUnits() const
{

}
} // namespace GameEngine
