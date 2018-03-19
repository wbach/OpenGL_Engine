#include "SingleStart.h"
#include "GameEngine/Engine/Engine.h"
#include "Scene/TestSceneFactory.h"

void TestGame::TestGameStart(GameEngine::IGraphicsApiPtr gptr)
{
	GameEngine::CEngine engine(gptr, std::make_shared<TestSceneFactory>());
	engine.Init();
	//engine.sceneManager_.AddScene(new MainScene(engine));
	engine.sceneManager_.SetActiveScene("MainScene");
	engine.GameLoop();
}
