#include "SkydomeShader.h"
#include "glm/gtx/transform.hpp"

namespace GameEngine
{
SkydomeShader::SkydomeShader(GraphicsApi::IGraphicsApi& graphicsApi)
    : ShaderProgram(graphicsApi, GraphicsApi::Shaders::Skydome)
{
}

void SkydomeShader::GetAllUniformLocations()
{
}

void SkydomeShader::ConnectTextureUnits() const
{
}

void SkydomeShader::BindAttributes() const
{
    BindAttribute(0, "Position");
}
}  // namespace GameEngine
