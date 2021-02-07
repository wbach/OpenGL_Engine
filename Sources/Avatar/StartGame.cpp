#include "StartGame.h"
#include "GameEngine/Engine/Engine.h"
#include "GameEngine/Physics/Bach/BachPhysicsAdapter.h"
#include "GameEngine/Physics/Bullet/BulletAdapter.h"
#include "Scene/SceneFactory.h"

using namespace GameEngine;
using namespace GameEngine::Physics;

namespace AvatarGame
{
void Start(std::unique_ptr<GraphicsApi::IGraphicsApi> gptr)
{
    Engine engine(std::move(gptr), std::make_unique<BulletAdapter>(), std::make_unique<SceneFactory>());
    engine.Init();
    engine.GetSceneManager().SetActiveScene("MainMenu");
    //engine.GetSceneManager().SetActiveScene("GreenIsland");
    engine.GameLoop();
}
}  // namespace AvatarGame
