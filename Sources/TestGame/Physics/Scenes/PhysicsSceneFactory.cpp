#include "PhysicsSceneFactory.h"
#include "PhysicsScene.h"

namespace PhysicsTestGame
{
PhysicsSceneFactory::PhysicsSceneFactory()
{
    AddScene("PhysicsScene", std::bind(&PhysicsSceneFactory::CreateMainScene, this));
}

GameEngine::ScenePtr PhysicsSceneFactory::CreateMainScene()
{
    return std::make_unique<PhysicsScene>();
}
}
