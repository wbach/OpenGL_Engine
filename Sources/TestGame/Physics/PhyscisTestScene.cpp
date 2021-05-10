#include "PhyscisTestScene.h"
#include "GameEngine/Engine/Engine.h"
#include "GameEngine/Physics/Bullet/BulletAdapter.h"
#include "Scenes/PhysicsSceneFactory.h"

using namespace GameEngine;
using namespace GameEngine::Physics;

namespace PhysicsTestGame
{
void Start(std::unique_ptr<GraphicsApi::IGraphicsApi> gptr)
{
    Engine engine(std::make_unique<Bullet::BulletAdapter>(), std::make_unique<PhysicsSceneFactory>());
    engine.Init();
    engine.GetSceneManager().SetActiveScene("PhysicsScene");
    engine.GameLoop();
}
}  // PhysicsTestGame
