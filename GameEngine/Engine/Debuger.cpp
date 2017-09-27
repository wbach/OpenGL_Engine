#include "Debuger.h"
#include <gl/glew.h>
#include "../Renderers/Terrain/Tesselation/TessellationTerrainRenderer.h"
#include "../GameEngine/Renderers/GUI/GuiRenderer.h"
#include "../GameEngine/Renderers/GUI/Text/GuiText.h"
#include <iostream>

CGUIRenderer gui_renderer;

CDebuger::CDebuger(CInputManager & input)
	: inputManager(input)
{
}

void CDebuger::TurnOnOff()
{
	isEnabled = !isEnabled;
}

void CDebuger::Execute()
{
	if (!isEnabled)
		return;

	if(!isInit)

	Keys();
	RenderAllasGrid();
}

void CDebuger::Render()
{
	if (!isEnabled)
		return;

	//std::cout << "*******************************************"
	//	<< "Debug : on, " << std::endl;

	//gui_renderer.Render(nullptr);
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

void CDebuger::Init()
{
	auto guiText = new CGuiText("GUI/consola.ttf", {640, 480});
	gui_renderer.AddElement(guiText);

	SGuiTextElement score;
	score.text = "Debug: on";
	score.colour = glm::vec3(0, 162.f / 255.f, 232.f / 255.f);

	score.position = glm::vec2(-0.9, 0.5);
	guiText->texts["Line_p1"] = score;

	gui_renderer.Init();

	isInit = true;
}
