#include "SingleStart.h"
#include "GameEngine/Engine/Engine.h"
#include "Scene/TestSceneFactory.h"
#include "GameEngine/Physics/Bullet/BulletAdapter.h"

using namespace GameEngine;
using namespace GameEngine::Physics;

void TestGame::TestGameStart(GameEngine::IGraphicsApiPtr gptr)
{
	CEngine engine(gptr, makeBulletAdapter(), std::make_shared<TestSceneFactory>());
	engine.Init();
	//engine.sceneManager_.AddScene(new MainScene(engine));
	engine.sceneManager_.SetActiveScene("MainScene");
	engine.GameLoop();
}
