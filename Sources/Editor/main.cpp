#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Engine/Engine.h"
#include "Logger/Log.h"
#include "OpenGLApi/OpenGLApi.h"

#ifndef USE_GNU
#include "DirectXApi/DirectXApi.h"
#endif

#include "GameEngine/Engine/Engine.h"
#include "GameEngine/Physics/Bach/BachPhysicsAdapter.h"
#include "GameEngine/Physics/Bullet/BulletAdapter.h"
#include "Scene/SceneFactory.h"

const std::string configFile = "./Conf.xml";

using namespace GameEngine;
using namespace GameEngine::Physics;

int main(int, char**)
{
    CLogger::Instance().EnableLogs();

    GameEngine::ReadFromFile(configFile);
    auto api                       = std::make_unique<OpenGLApi::OpenGLApi>();
    GraphicsApi::IGraphicsApi& ptr = *api;

    // GameEngine::ReadFromFile("./ConfDx11.xml"); auto api = std::make_unique<DirectX::DirectXApi>();

    Engine engine(std::move(api), std::make_unique<BulletAdapter>(ptr), std::make_shared<Editor::SceneFactory>());
    engine.Init();
    engine.GetSceneManager().SetActiveScene("ChooseEditor");
    engine.GameLoop();

    return 0;
}
