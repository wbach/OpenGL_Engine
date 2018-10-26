#include "GuiTextureShader.h"

namespace GameEngine
{
namespace Renderer
{
namespace Gui
{
GuiTextureShader::GuiTextureShader(IGraphicsApiPtr graphicsApi)
    : ShaderProgram(graphicsApi)
{
}
void GuiTextureShader::Init()
{
    SetFiles({{"Gui/guiVertexShader.vert", ShaderType::VERTEX_SHADER},
              {"Gui/guiFragmentShader.frag", ShaderType::FRAGMENT_SHADER}});

    ShaderProgram::Init();
}
void GuiTextureShader::GetAllUniformLocations()
{
    location_transformationMatrix = GetUniformLocation("transformationMatrix");
}
void GuiTextureShader::BindAttributes()
{
    BindAttribute(0, "position");
}
void GuiTextureShader::LoadTransformMatrix(const glm::mat4& matrix) const
{
    LoadValue(location_transformationMatrix, matrix);
}
}  // Gui
}  // Renderer
}  // GameEngine
