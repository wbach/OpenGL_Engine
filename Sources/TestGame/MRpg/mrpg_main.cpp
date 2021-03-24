#include "mrpg_main.h"
#include <Common/Messages/BinaryMessageConverter.h>
#include <Logger/Log.h>
#include <UtilsNetwork/Gateway.h>
#include "Common/Hero/HeroClassesTypes.h"
#include "Common/MessageHandling/Dispatcher.h"
#include "Engine/Engine.h"
#include "GameEngine/Physics/Bach/BachPhysicsAdapter.h"
#include "GameEngine/Physics/Bullet/BulletAdapter.h"
#include "Utils/FileSystem/FileSystemUtils.hpp"
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
    Game()
        : serverAdress_(Utils::ReadFile("./server.conf"))
        , engine_(std::make_unique<BachPhysicsAdapter>(), std::make_unique<SceneFactory>(gateway_, serverAdress_, gameContext_))
    {
        DEBUG_LOG("Server : " + serverAdress_);
        gateway_.AddMessageConverter(std::make_unique<common::BinaryMessageConverter>());

        engine_.Init();
        // LoginScene
        // engine.sceneManager_.SetActiveScene("GuiEdytorScene");
        engine_.GetSceneManager().SetActiveScene("LoginScene");
        // engine.sceneManager_.SetActiveScene("SelectCharacterScene");
        engine_.GameLoop();
    }

private:
    std::string serverAdress_;
    Network::Gateway gateway_;
    Engine engine_;
    MrpgGameContext gameContext_;
};

void StartGame()
{
    Game game();
    // Mock::StartMock();
}
}  // namespace MmmoRpg
