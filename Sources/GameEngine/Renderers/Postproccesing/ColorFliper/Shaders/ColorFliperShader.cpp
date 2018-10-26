#include "ColorFliperShader.h"

#define GetLocation(X) uniformLocations[UniformLocation::X] = GetUniformLocation(#X)

namespace GameEngine
{
ColorFliperShader::ColorFliperShader(IGraphicsApiPtr graphicsApi)
    : ShaderProgram(graphicsApi)
{
    SetFiles({
        {"PostProcess/ColorFliper/ColorFliper.vert", GameEngine::ShaderType::VERTEX_SHADER},
        {"PostProcess/ColorFliper/ColorFliper.frag", GameEngine::ShaderType::FRAGMENT_SHADER},
    });

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
    LoadValue(uniformLocations.at(ColorMap), 0);
}
}  // GameEngine
