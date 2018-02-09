#include "TestSceneFactory.h"
#include "MainScene.h"

TestSceneFactory::TestSceneFactory()
{
	AddScene("MainScene", std::bind(&TestSceneFactory::CreateMainScene, this));
}

GameEngine::ScenePtr TestSceneFactory::CreateMainScene()
{
	return std::make_unique<MainScene>();
}
