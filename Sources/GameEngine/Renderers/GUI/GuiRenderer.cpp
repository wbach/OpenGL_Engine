#include "GuiRenderer.h"

namespace GameEngine
{
	GUIRenderer::GUIRenderer(std::function<void(RendererFunctionType, RendererFunction)> rendererFunction)
	{
		rendererFunction(RendererFunctionType::POSTUPDATE, std::bind(&GUIRenderer::Render, this, std::placeholders::_1));
	}
	void GUIRenderer::Init()
	{
		for (const auto& element : guiElements)
		{
			element->Init();
		}
	}

	void GUIRenderer::Render(Scene* scene)
	{
		for (const auto& element : guiElements)
		{
			element->Render();
		}
	}

	void GUIRenderer::UnSubscribeAll()
	{
		for (auto& el : guiElements)
			el->UnSubscribeAll();
	}
	void GUIRenderer::ReloadShaders()
	{
		for (auto& el : guiElements)
			el->ReloadShaders();
	}
	void GUIRenderer::AddElement(GuiElement * element)
	{
		guiElements.emplace_back(element);
		element->Init();
	}
} // GameEngine
