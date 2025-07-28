#include "StartGame.h"

#include "Game/PauseMenuComponent.h"
#include "GameEngine/Engine/Engine.h"
#include "GameEngine/Physics/Bach/BachPhysicsAdapter.h"
#include "GameEngine/Physics/Bullet/BulletAdapter.h"
#include "Scene/SceneFactory.h"

using namespace GameEngine;
using namespace GameEngine::Physics;

namespace AvatarGame
{
void Start()
{
    Engine engine(std::make_unique<Bullet::BulletAdapter>(), std::make_unique<SceneFactory>());
    engine.Init();
    PauseMenuComponent::registerReadFunctions();
    engine.GetSceneManager().SetActiveScene("MainMenu");
    // engine.GetSceneManager().SetActiveScene("Kingdom");
    // engine.GetSceneManager().SetActiveScene("GreenIsland");
    engine.GameLoop();
}
}  // namespace AvatarGame
