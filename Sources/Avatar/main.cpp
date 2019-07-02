#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Engine/Engine.h"
#include "Logger/Log.h"
#include "OpenGLApi/OpenGLApi.h"
#include "StartGame.h"

#ifndef USE_GNU
#include "DirectXApi/DirectXApi.h"
#endif

const std::string configFile = "./Conf.xml";

int main(int, char**)
{
    CLogger::Instance().EnableLogs();

    GameEngine::ReadFromFile(configFile);
    auto api = std::make_unique<OpenGLApi::OpenGLApi>();
    // GameEngine::ReadFromFile("./ConfDx11.xml"); auto api = std::make_unique<DirectX::DirectXApi>();
    AvatarGame::Start(std::move(api));
    return 0;
}
