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
    GraphicsApi::IGraphicsApi& ptr = *gptr;
    Engine engine(std::move(gptr), std::make_unique<BulletAdapter>(ptr), std::make_unique<SceneFactory>());
    engine.Init();
    engine.GetSceneManager().SetActiveScene("MainMenu");
    engine.GameLoop();
}
}  // namespace AvatarGame
