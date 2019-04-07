#include "GuiTextureShader.h"
#include "GuiTextureShaderUniforms.h"

namespace GameEngine
{
namespace Renderer
{
namespace Gui
{
GuiTextureShader::GuiTextureShader(GraphicsApi::IGraphicsApi& graphicsApi)
    : ShaderProgram(graphicsApi, GraphicsApi::Shaders::Texture)
{
}

void GuiTextureShader::GetAllUniformLocations()
{
    uniforms_.resize(GuiTextureShaderUniforms::SIZE);
    uniforms_[GuiTextureShaderUniforms::TransformationMatrix] = GetUniformLocation("transformationMatrix");
    uniforms_[GuiTextureShaderUniforms::color]                = GetUniformLocation("color");
}
void GuiTextureShader::BindAttributes()
{
    BindAttribute(0, "position");
}
}  // namespace Gui
}  // namespace Renderer
}  // namespace GameEngine
