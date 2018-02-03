#pragma once
#include "Renderer.h"
#include "Projection.h"
#include "GUI/GuiContext.h"

struct SGuiTextElement;

namespace GameEngine
{
	class Scene;

	namespace Renderer
	{
		namespace Gui
		{
			class GuiTextureElement;
		} // Gui

		class RenderersManager
		{
		public:
			RenderersManager();
			void Init();
			const CProjection& GetProjection() const;
			void RenderScene(Scene* scene);
			void Subscribe(CGameObject* gameObject);
			void UnSubscribeAll();
			SGuiTextElement& GuiText(const std::string& label);
			Gui::GuiTextureElement& GuiTexture(const std::string& label);

		private:
			void InitProjection();
			void InitMainRenderer();
			void InitGuiRenderer();
			void Render(Scene* scene, CRenderer* renderer);

		private:
			Renderer::Gui::GuiContext guiContext_;
			RenderersPtrVec renderers_;
			CProjection projection_;
		};
	} // Renderer
} // GameEngine