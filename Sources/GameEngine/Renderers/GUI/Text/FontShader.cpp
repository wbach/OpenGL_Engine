#include "FontShader.h"
#include "FontShaderUniforms.h"

namespace GameEngine
{
FontShader::FontShader(GraphicsApi::IGraphicsApi& graphicsApi)
    : ShaderProgram(graphicsApi, GraphicsApi::Shaders::Font)
{
}

void FontShader::GetAllUniformLocations()
{
}

void FontShader::BindAttributes()
{
    BindAttribute(0, "position");
}
}  // namespace GameEngine
