#pragma once
#include "Scene/SceneFactoryBase.h"

class TestSceneFactory : public GameEngine::SceneFactoryBase
{
public:
	TestSceneFactory();

private:
	GameEngine::ScenePtr CreateMainScene();
};
