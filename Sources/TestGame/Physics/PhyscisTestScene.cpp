#include "PhyscisTestScene.h"
#include "GameEngine/Engine/Engine.h"
#include "GameEngine/Physics/Bullet/BulletAdapter.h"
#include "Scenes/PhysicsSceneFactory.h"

using namespace GameEngine;
using namespace GameEngine::Physics;

namespace PhysicsTestGame
{
void Start(GameEngine::IGraphicsApiPtr gptr)
{
    Engine engine(gptr, std::make_shared<BulletAdapter>(), std::make_shared<PhysicsSceneFactory>());
    engine.Init();
    engine.sceneManager_.SetActiveScene("PhysicsScene");
    engine.GameLoop();
}
}  // PhysicsTestGame
