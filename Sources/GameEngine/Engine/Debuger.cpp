#include "Debuger.h"
#include <GL/glew.h>
#include "../Renderers/Terrain/Tesselation/TessellationTerrainRenderer.h"
#include "../GameEngine/Renderers/GUI/GuiRenderer.h"
#include "../GameEngine/Renderers/GUI/Text/GuiText.h"
#include <iostream>

CDebuger::CDebuger(CInputManager & input)
	: inputManager(input)
	, gui_renderer(nullptr)
	, guiText(nullptr)
{
}

void CDebuger::TurnOnOff()
{
	isEnabled = !isEnabled;
}

void CDebuger::Execute()
{
	if (inputManager.GetKeyDown(KeyCodes::V))
	{
		TurnOnOff();
	}

	if (!isEnabled)
		return;

	UpdateCommandText();

	Keys();
	RenderAllasGrid();

	for (auto& action : actions)
	{
		if (inputManager.GetKeyDown(action.first))
		{
			action.second();
		}
	}

}

void CDebuger::Render()
{
	if (!isEnabled)
		return;

	if (!isSetGuiRenderer())
		return;

	//std::cout << "*******************************************"
	//	<< "Debug : on, " << std::endl;

	//gui_renderer.Render(nullptr);
}

void CDebuger::AddAction(KeyCodes::Type key, std::function<void()> action)
{
	actions[key] = action;
}

void CDebuger::SetGuiRenderer(CGUIRenderer * renderer)
{
	gui_renderer = renderer;
}

void CDebuger::RenderAllasGrid()
{
	if (!renderAsGrid)
		return;

	glPolygonMode(GL_FRONT, GL_LINE);
	glPolygonMode(GL_BACK, GL_LINE);	
}

void CDebuger::Keys()
{
	if (inputManager.GetKeyDown(KeyCodes::G))
	{
		renderAsGrid = !renderAsGrid;
	}

	if (inputManager.GetKey(KeyCodes::Z))
	{
		CTessellationTerrainRenderer::distanceDev += 0.1f;
	}

	if (inputManager.GetKey(KeyCodes::X))
	{
		CTessellationTerrainRenderer::distanceDev -= 0.1f;
	}

	if (inputManager.GetKey(KeyCodes::N))
	{
		CTessellationTerrainRenderer::minTessLevelOuter++;
	}

	if (inputManager.GetKey(KeyCodes::M))
	{
		CTessellationTerrainRenderer::minTessLevelOuter--;
	}

	if (inputManager.GetKey(KeyCodes::L))
	{
		std::cout << CTessellationTerrainRenderer::minTessLevelOuter << " " << CTessellationTerrainRenderer::distanceDev << std::endl;
	}
}

void CDebuger::UpdateCommandText()
{
	if (guiText == nullptr)
		return;
	auto& text = guiText->texts["comandsLine"].text;
	text.clear();
	for (const auto& c : commandsHistory)
	{
		text += c;
		text += "\n";
	}
}

void CDebuger::Init()
{
	guiText = new CGuiText("GUI/consola.ttf", {640, 480});
	gui_renderer->AddElement(guiText);

	SGuiTextElement score;
	score.text = "";
	score.m_size = 0.5;
	score.colour = glm::vec3(0, 162.f / 255.f, 232.f / 255.f);

	score.position = glm::vec2(-0.95, 0.0);
	guiText->texts["comandsLine"] = score;

	isInit = true;
}

bool CDebuger::isSetGuiRenderer() const
{
	return gui_renderer != nullptr;
}
