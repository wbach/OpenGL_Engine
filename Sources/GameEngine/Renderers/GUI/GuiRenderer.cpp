#include "GuiRenderer.h"

void CGUIRenderer::Init()
{
    for (const auto& element : guiElements)
	{
		element->Init();
	}
}

void CGUIRenderer::PrepareFrame(CScene * scene)
{
}

void CGUIRenderer::Render(CScene * scene)
{
    for (const auto& element : guiElements)
	{
		element->Render();
	}
}

void CGUIRenderer::EndFrame(CScene * scene)
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
