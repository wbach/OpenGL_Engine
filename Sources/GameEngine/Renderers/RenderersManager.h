#pragma once
#include "Renderer.h"
#include "Projection.h"
#include "GUI/GuiContext.h"

class CScene;
struct SGuiTextElement;

namespace GameEngine
{
	namespace Renderer
	{
		namespace Gui
		{
			struct GuiTextureElement;
		} // Gui

		class RenderersManager
		{
		public:
			RenderersManager();
			void Init();
			const CProjection& GetProjection() const;
			void RenderScene(CScene* scene);
			void Subscribe(CGameObject* gameObject);
			void UnSubscribeAll();
			SGuiTextElement& GuiText(const std::string& label);
			Gui::GuiTextureElement& GuiTexture(const std::string& label);

		private:
			void InitProjection();
			void InitMainRenderer();
			void InitGuiRenderer();
			void Render(CScene* scene, CRenderer* renderer);

		private:
			Renderer::Gui::GuiContext guiContext_;
			RenderersPtrVec renderers_;
			CProjection projection_;
		};
	} // Renderer
} // GameEngine