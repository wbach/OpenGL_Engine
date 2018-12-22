#include "ColorFliperShader.h"
#include "ColorFliperUniformLocation.h"

#define GetLocation(X) uniforms_[ColorFilperUniformLocation::X] = GetUniformLocation(#X)

namespace GameEngine
{
ColorFliperShader::ColorFliperShader(IGraphicsApiPtr graphicsApi)
    : ShaderProgram(graphicsApi, graphicsApi->GetShaderFiles(Shaders::ColorFilper))
{
    ShaderProgram::Init();
}
void ColorFliperShader::BindAttributes()
{
    BindAttribute(0, "Position");
    BindAttribute(1, "TexCoord");
}
void ColorFliperShader::GetAllUniformLocations()
{
    GetLocation(ColorMap);
}
void ColorFliperShader::ConnectTextureUnits() const
{
    Load(ColorFilperUniformLocation::ColorMap, 0);
}
}  // GameEngine
