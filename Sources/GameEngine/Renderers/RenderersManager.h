#pragma once
#include "Renderer.h"
#include "Projection.h"
#include "GUI/GuiContext.h"
#include <atomic>
#include <list>

struct SGuiTextElement;
class CGameObject;

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
			void ReloadShaders();
			void Subscribe(CGameObject* gameObject);
			void UnSubscribe(CGameObject* gameObject);
			void UnSubscribeAll();
			void SwapLineFaceRender();
			SGuiTextElement& GuiText(const std::string& label);
			Gui::GuiTextureElement& GuiTexture(const std::string& label);

		private:
			void ReloadShadersExecution();
			void UpdateCamera(Scene* scene);
			void TakeSnapShoots();
			void InitProjection();
			void InitMainRenderer();
			void InitGuiRenderer();
			void Render(Scene* scene, CRenderer* renderer);
			
		private:
			std::atomic_bool renderAsLines;
			std::atomic_bool markToReloadShaders_;
			Renderer::Gui::GuiContext guiContext_;
			RenderersPtrVec renderers_;
			CProjection projection_;
			std::list<CGameObject*> dynamincObjects_;
		};
	} // Renderer
} // GameEngine