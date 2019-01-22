#include "SingleStart.h"
#include "GameEngine/Engine/Engine.h"
#include "GameEngine/Physics/Bullet/BulletAdapter.h"
#include "GameEngine/Physics/Bach/BachPhysicsAdapter.h"
#include "Scene/TestSceneFactory.h"

using namespace GameEngine;
using namespace GameEngine::Physics;

void TestGame::TestGameStart(std::unique_ptr<GraphicsApi::IGraphicsApi> gptr)
{
    GraphicsApi::IGraphicsApi& ptr = *gptr;
    Engine engine(std::move(gptr), std::make_unique<BulletAdapter>(ptr), std::make_shared<TestSceneFactory>());
    engine.Init();
    // engine.sceneManager_.AddScene(new MainScene(engine));
    engine.GetSceneManager().SetActiveScene("MainScene");
    engine.GameLoop();
}
