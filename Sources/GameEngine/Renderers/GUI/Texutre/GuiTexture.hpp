#pragma once
#include "../GuiElement.h"
#include "GuiTextureElement.h"
#include "GuiTextureShader.h"
#include "../../Resources/Textures/Texture.h"
#include "../../Resources/SimpleModels/Quad.h"
#include <unordered_map>

namespace GameEngine
{
	namespace Renderer
	{
		namespace Gui
		{
			typedef std::unordered_map<std::string, GuiTextureElement> GuiTextures;

			class CGuiTexture : public CGuiElement
			{
			public:
				virtual ~CGuiTexture() override;
				virtual void Init() override;
				virtual void Render() override;
				virtual void UnSubscribeAll() override;
				GuiTextures guiTextures_;

			private:
				void RenderTextureElement(const GuiTextureElement& te);

			private:
				SSimpleQuad quad_;
				GuiTextureShader shader_;
			};
		} // Gui
	} // Renderer
} // GameEngine