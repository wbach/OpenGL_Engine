#include "GuiRenderer.h"

void CGUIRenderer::Init()
{
	for (const auto& element : m_GuiElements)
	{
		element->Init();
	}
}

void CGUIRenderer::PrepareFrame(CScene * scene)
{
}

void CGUIRenderer::Render(CScene * scene)
{
	for (const auto& element : m_GuiElements)
	{
		element->Render();
	}
}

void CGUIRenderer::EndFrame(CScene * scene)
{
}


void CGUIRenderer::AddElement(CGuiElement * element)
{
	m_GuiElements.emplace_back(element);
}
