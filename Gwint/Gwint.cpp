#include "Gwint.h"
#include "Network/SDLClientGetway.h"
#include "Messages/Messages.h"

void CGwintGame::Start()
{
	engine.m_Projection = SProjection(wb::vec2i{ 640, 600 }, 0.01f, 100.f, 50);
	renderer = new CGwitRenderer(&engine.m_Projection);
	engine.m_Renderers.emplace_back(renderer);

	engine.Init();
	scene = new GwintGameScene(engine);
	engine.m_Scene = std::unique_ptr<GwintGameScene>(scene);

	renderer->renderer_quad = new CEntity(engine.m_Scene->GetResourceManager(), "../Data/Gwint/RenderQuad.obj", glm::vec3(0, 0, 0));
	renderer->renderer_quad->m_WorldTransform.SetScale(glm::vec3(-1, 1, 1));
	renderer->renderer_quad->m_WorldTransform.SetPosition(glm::vec3(0, 0, -2));
	renderer->bg_texture = engine.m_Scene->GetResourceManager().GetTextureLaoder().LoadTexture("../Data/Gwint/gwint_table.jpg");

	scene->renderer = renderer;

	NetworkStartProcedure();

	//GameCards::Instance().LoadCards();
	GameCards::Instance().LoadTexturesInCards(engine.m_Scene->GetResourceManager());

	for (const auto& decks : GameCards::Instance().GetCards())
	{
		for (const auto& card : decks.second)
		{
			scene->player.cards_in_deck.push_back(card);
		}
	}


	engine.PreperaScene();
	engine.GameLoop();
}

void CGwintGame::NetworkStartProcedure()
{	
	std::cout << "Connecting to server..." << std::endl;
	SDLClientGetway::Instance().Init();
}


