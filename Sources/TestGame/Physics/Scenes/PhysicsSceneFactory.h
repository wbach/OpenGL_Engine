#pragma once
#include "Scene/SceneFactoryBase.h"

namespace Network
{
class CGateway;
}  // Network

namespace PhysicsTestGame
{
class PhysicsSceneFactory : public GameEngine::SceneFactoryBase
{
public:
    PhysicsSceneFactory();

private:
    GameEngine::ScenePtr CreateMainScene();
};
}  // PhysicsTestGame
