#include "GuiRenderer.h"

void CGUIRenderer::Init()
{
    for (const auto& element : guiElements)
	{
		element->Init();
	}
}

void CGUIRenderer::PrepareFrame(GameEngine::Scene* scene)
{
}

void CGUIRenderer::Render(GameEngine::Scene* scene)
{
    for (const auto& element : guiElements)
	{
		element->Render();
	}
}

void CGUIRenderer::EndFrame(GameEngine::Scene* scene)
{
}

void CGUIRenderer::UnSubscribeAll()
{
	for (auto& el : guiElements)
		el->UnSubscribeAll();
}


void CGUIRenderer::AddElement(CGuiElement * element)
{
    guiElements.emplace_back(element);
	element->Init();
}
