#include "GuiTextureShader.h"

namespace GameEngine
{
	namespace Renderer
	{
		namespace Gui
		{
			void GuiTextureShader::Init()
			{
				SetFiles
				({
					{ "Gui/guiVertexShader.vert", GL_VERTEX_SHADER },
					{ "Gui/guiFragmentShader.frag", GL_FRAGMENT_SHADER }
				});

				CShaderProgram::Init();
			}
			void GuiTextureShader::GetAllUniformLocations()
			{
				location_transformationMatrix = GetUniformLocation("transformationMatrix");
			}
			void GuiTextureShader::BindAttributes()
			{
				BindAttribute(0, "position");
			}
			void GuiTextureShader::LoadTransformMatrix(const glm::mat4 & matrix) const
			{
				LoadValue(location_transformationMatrix, matrix);
			}
		} // Gui
	} // Renderer
} // GameEngine