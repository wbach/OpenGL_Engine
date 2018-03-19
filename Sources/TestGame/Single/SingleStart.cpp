#include "SingleStart.h"
#include "GameEngine/Engine/Engine.h"
#include "GameEngine/Api/OpenGL/OpenGLApi.h"
#include "Scene/TestSceneFactory.h"

void TestGame::TestGameStart()
{
	GameEngine::CEngine engine(std::make_shared<GameEngine::OpenGLApi>(), std::make_shared<TestSceneFactory>());
	engine.Init();
	//engine.sceneManager_.AddScene(new MainScene(engine));
	engine.sceneManager_.SetActiveScene("MainScene");
	engine.GameLoop();
}
