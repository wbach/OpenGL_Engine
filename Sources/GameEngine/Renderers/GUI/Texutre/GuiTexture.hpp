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

			class GuiTexture : public GuiElement
			{
			public:
				GuiTexture(IGraphicsApiPtr graphicsApi);
				virtual ~GuiTexture() override;
				virtual void Init() override;
				virtual void Render() override;
				virtual void UnSubscribeAll() override;
				virtual void ReloadShaders() override;
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