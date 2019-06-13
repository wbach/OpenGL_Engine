#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Engine/Engine.h"
#include "Logger/Log.h"
#include "OpenGLApi/OpenGLApi.h"
#include "StartGame.h"

#ifndef USE_GNU
#include "DirectXApi/DirectXApi.h"
#endif

int main(int, char**)
{
    GameEngine::ReadFromFile("./Conf.xml");
    auto api = std::make_unique<OpenGLApi::OpenGLApi>();
    // GameEngine::ReadFromFile("./ConfDx11.xml"); auto api = std::make_unique<DirectX::DirectXApi>();
    CLogger::Instance().EnableLogs();
    AvatarGame::Start(std::move(api));
    return 0;
}
