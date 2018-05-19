#pragma once
#include "GuiElement.h"
#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Renderers/RendererFunctionType.h"
#include <memory>
#include <vector>

namespace GameEngine
{
	class GUIRenderer : public IRenderer
	{
	public:
		GUIRenderer(std::function<void(RendererFunctionType, RendererFunction)> rendererFunction);
		virtual ~GUIRenderer() {}
		virtual void Init() override;
		virtual void Subscribe(CGameObject* gameObject) {}
		virtual void UnSubscribeAll() override;
		virtual void ReloadShaders() override;
		// create unique_ptr from ptr
		void AddElement(GuiElement* element);

	private:
		void Render(Scene* scene);

	private:
		std::vector<GuiElementPtr> guiElements;
	};
} // GameEngine
