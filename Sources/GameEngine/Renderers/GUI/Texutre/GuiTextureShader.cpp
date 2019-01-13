#include "GuiTextureShader.h"
#include "GuiTextureShaderUniforms.h"

namespace GameEngine
{
namespace Renderer
{
namespace Gui
{
GuiTextureShader::GuiTextureShader(IGraphicsApi& graphicsApi)
    : ShaderProgram(graphicsApi, Shaders::Texture)
{
}

void GuiTextureShader::GetAllUniformLocations()
{
    uniforms_.resize(GuiTextureShaderUniforms::SIZE);
    uniforms_[GuiTextureShaderUniforms::TransformationMatrix] = GetUniformLocation("transformationMatrix");
}
void GuiTextureShader::BindAttributes()
{
    BindAttribute(0, "position");
}
}  // namespace Gui
}  // namespace Renderer
}  // namespace GameEngine
