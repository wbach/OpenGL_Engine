#include "GridShader.h"
#include "GridShaderUniforms.h"

namespace GameEngine
{
GridShader::GridShader(GraphicsApi::IGraphicsApi& graphicsApi)
    : ShaderProgram(graphicsApi, GraphicsApi::Shaders::Grid)
{
}

void GridShader::GetAllUniformLocations()
{
}
void GridShader::BindAttributes()
{
    BindAttribute(0, "Position");
    BindAttribute(1, "TexCoord");
    BindAttribute(2, "Normal");
    BindAttribute(3, "Tangent");
    BindAttribute(4, "Weights");
    BindAttribute(5, "BoneIds");
}
void GridShader::ConnectTextureUnits() const
{
}
}  // namespace GameEngine
