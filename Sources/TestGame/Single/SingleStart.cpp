#include "SingleStart.h"
#include "GameEngine/Engine/Engine.h"
#include "GameEngine/Physics/Bullet/BulletAdapter.h"
#include "GameEngine/Physics/Bach/BachPhysicsAdapter.h"
#include "Scene/TestSceneFactory.h"

using namespace GameEngine;
using namespace GameEngine::Physics;

void TestGame::TestGameStart(GameEngine::IGraphicsApiPtr gptr)
{
    Engine engine(gptr, std::make_shared<BachPhysicsAdapter>(), std::make_shared<TestSceneFactory>());
    engine.Init();
    // engine.sceneManager_.AddScene(new MainScene(engine));
    engine.sceneManager_.SetActiveScene("MainScene");
    engine.GameLoop();
}
