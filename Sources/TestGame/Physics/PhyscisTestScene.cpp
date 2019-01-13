#include "PhyscisTestScene.h"
#include "GameEngine/Engine/Engine.h"
#include "GameEngine/Physics/Bullet/BulletAdapter.h"
#include "Scenes/PhysicsSceneFactory.h"

using namespace GameEngine;
using namespace GameEngine::Physics;

namespace PhysicsTestGame
{
void Start(std::unique_ptr<GameEngine::IGraphicsApi> gptr)
{
    IGraphicsApi& ptr = *gptr;
    Engine engine(std::move(gptr), std::make_unique<BulletAdapter>(ptr), std::make_shared<PhysicsSceneFactory>());
    engine.Init();
    engine.GetSceneManager().SetActiveScene("PhysicsScene");
    engine.GameLoop();
}
}  // PhysicsTestGame
