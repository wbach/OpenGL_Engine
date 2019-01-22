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
    uniforms_.resize(FontShaderUniforms::SIZE);

    uniforms_[FontShaderUniforms::Color]                = GetUniformLocation("color");
    uniforms_[FontShaderUniforms::TextSize]             = GetUniformLocation("textSize");
    uniforms_[FontShaderUniforms::Translation]          = GetUniformLocation("translation");
    uniforms_[FontShaderUniforms::TransformationMatrix] = GetUniformLocation("transformationMatrix");
}

void FontShader::BindAttributes()
{
}
}  // namespace GameEngine
