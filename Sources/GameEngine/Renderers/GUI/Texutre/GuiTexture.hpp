#pragma once
#include "../GuiElement.h"
#include "GuiTextureElement.h"
#include "GuiTextureShader.h"
#include "GameEngine/Resources/Textures/Texture.h"
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
				CGuiTexture(IGraphicsApiPtr graphicsApi);
				virtual ~CGuiTexture() override;
				virtual void Init() override;
				virtual void Render() override;
				virtual void UnSubscribeAll() override;
				GuiTextures guiTextures_;

			private:
				void RenderTextureElement(const GuiTextureElement& te);

			private:
				IGraphicsApiPtr graphicsApi_;
				GuiTextureShader shader_;
			};
		} // Gui
	} // Renderer
} // GameEngine