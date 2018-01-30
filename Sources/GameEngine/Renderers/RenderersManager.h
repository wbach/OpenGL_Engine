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

		private:
			void InitProjection();
			void InitMainRenderer();
			void InitGuiRenderer();
			void Render(CScene* scene, CRenderer* renderer);

		private:
			GUI::GuiContext guiContext_;
			RenderersPtrVec renderers_;
			CProjection projection_;
		};
	} // Renderer
} // GameEngine