#include "mrpg_main.h"
#include <UtilsNetwork/Gateway.h>
#include "Common/Hero/HeroClassesTypes.h"
#include "Common/MessageHandling/Dispatcher.h"
#include "Engine/Engine.h"
#include "GameEngine/Physics/Bach/BachPhysicsAdapter.h"
#include "GameEngine/Physics/Bullet/BulletAdapter.h"
#include "Logger/Log.h"
#include "MrpgGameContext.h"
#include "PlayerMock.h"
#include "Scenes/ScenesFactory.h"

using namespace GameEngine;
using namespace GameEngine::Physics;

namespace MmmoRpg
{
class Game
{
   public:
    Game(std::unique_ptr<GraphicsApi::IGraphicsApi> gptr)
        : serverAdress(Utils::ReadFile("./server.conf"))
        , engine(std::move(gptr), std::make_unique<BachPhysicsAdapter>(), std::make_shared<SceneFactory>(gateway, serverAdress, gameContext_))
    {
        DEBUG_LOG("Server : " + serverAdress);

        engine.Init();
        // LoginScene
        // engine.sceneManager_.SetActiveScene("GuiEdytorScene");
        engine.GetSceneManager().SetActiveScene("LoginScene");
        // engine.sceneManager_.SetActiveScene("SelectCharacterScene");
        engine.GameLoop();
    }

   private:
    std::string serverAdress;
    Network::Gateway gateway;
    Engine engine;
    MrpgGameContext gameContext_;
};

void StartGame(std::unique_ptr<GraphicsApi::IGraphicsApi> gptr)
{
    Game game(std::move(gptr));
    // Mock::StartMock();
}
}  // namespace MmmoRpg
