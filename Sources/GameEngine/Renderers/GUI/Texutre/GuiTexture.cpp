#include "GuiTexture.hpp"

namespace GameEngine
{
	namespace Renderer
	{
		namespace Gui
		{
			GuiTexture::GuiTexture(IGraphicsApiPtr graphicsApi)
				: graphicsApi_(graphicsApi)
				, shader_(graphicsApi)
			{
			}
			GuiTexture::~GuiTexture()
			{
			}
			void GuiTexture::Init()
			{
				shader_.Init();
			}
			void GuiTexture::Render()
			{
				shader_.Start();
				for (const auto& gte : guiTextures_)
					RenderTextureElement(gte.second);
				shader_.Stop();
			}
			void GuiTexture::UnSubscribeAll()
			{
				guiTextures_.clear();
			}
			void GuiTexture::RenderTextureElement(const GuiTextureElement& te)
			{
				shader_.LoadTransformMatrix(te.GetMatrix());
				graphicsApi_->ActiveTexture(0, te.texture->GetId());
				graphicsApi_->RenderQuad();
			}
			void GuiTexture::ReloadShaders()
			{
				shader_.Stop();
				shader_.Reload();
				shader_.Init();
			}
		} // Gui
	} // Renderer
} // GameEngine