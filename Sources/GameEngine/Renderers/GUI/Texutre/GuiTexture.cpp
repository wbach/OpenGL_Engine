#include "GuiTexture.hpp"

namespace GameEngine
{
	namespace Renderer
	{
		namespace Gui
		{
			CGuiTexture::CGuiTexture(IGraphicsApiPtr graphicsApi)
				: graphicsApi_(graphicsApi)
				, shader_(graphicsApi)
			{
			}
			CGuiTexture::~CGuiTexture()
			{
			}
			void CGuiTexture::Init()
			{
				shader_.Init();
			}
			void CGuiTexture::Render()
			{
				shader_.Start();
				for (const auto& gte : guiTextures_)
					RenderTextureElement(gte.second);
				shader_.Stop();
			}
			void CGuiTexture::UnSubscribeAll()
			{
				guiTextures_.clear();
			}
			void CGuiTexture::RenderTextureElement(const GuiTextureElement& te)
			{
				shader_.LoadTransformMatrix(te.GetMatrix());
				graphicsApi_->ActiveTexture(0, te.texture->GetId());
				graphicsApi_->RenderQuad();
			}
		} // Gui
	} // Renderer
} // GameEngine