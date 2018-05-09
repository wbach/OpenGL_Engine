#pragma once
#include <vector>
#include "GameEngine/Shaders/ShaderProgram.h"

namespace GameEngine
{
	namespace Renderer
	{
		namespace Gui
		{
			class GuiTextureShader : public CShaderProgram
			{
			public:
				GuiTextureShader(IGraphicsApiPtr graphicsApi);
				void Init();
				void GetAllUniformLocations() override;
				void BindAttributes() override;
				void LoadTransformMatrix(const glm::mat4& matrix) const;

			private:
				int location_transformationMatrix;
			};
		} // Gui
	} // Renderer
} // GameEngine