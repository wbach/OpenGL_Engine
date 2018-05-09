#include "DefferedShader.h"

namespace GameEngine
{
DefferedShader::DefferedShader(IGraphicsApiPtr graphicsApi)
	: CShaderProgram(graphicsApi)
{
}

void DefferedShader::GetAllUniformLocations()
{
}
void DefferedShader::BindAttributes()
{
}
void DefferedShader::ConnectTextureUnits() const
{
}
}// GameEngine
