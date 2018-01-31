#include "GuiTexture.hpp"

namespace GameEngine
{
	namespace Renderer
	{
		namespace Gui
		{
			CGuiTexture::~CGuiTexture()
			{
			}
			void CGuiTexture::Init()
			{
				quad_.Init();
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
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, te.texture->GetId());
				shader_.LoadTransformMatrix(te.transformMatrix_);
				Utils::SimpleRenderVao(quad_.vao, quad_.indicesSize, 2);
			}
		} // Gui
	} // Renderer
} // GameEngine