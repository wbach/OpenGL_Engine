#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Engine/Engine.h"
#include "Logger/Log.h"
#include "OpenGLApi/OpenGLApi.h"
#include "StartGame.h"
#include <Utils/FileSystem/FileSystemUtils.hpp>
#ifndef USE_GNU
#include "DirectXApi/DirectXApi.h"
#endif

const std::string configFile = "./Conf.xml";

int main(int, char**)
{
    Utils::PrintFilesInDirectory(".");
    return 0;
    CLogger::Instance().EnableLogs();

    GameEngine::ReadFromFile(configFile);
    std::unique_ptr<GraphicsApi::IGraphicsApi> graphicsApi;

#ifndef USE_GNU
    if (EngineConf.renderer.graphicsApi == "OpenGL")
    {
        graphicsApi = std::make_unique<OpenGLApi::OpenGLApi>();
    }
    else if (EngineConf.renderer.graphicsApi == "DirectX11")
    {
        graphicsApi = std::make_unique<DirectX::DirectXApi>();
    }
    else
    {
        graphicsApi = std::make_unique<OpenGLApi::OpenGLApi>();
    }
#else
    if (EngineConf.renderer.graphicsApi != "OpenGL")
    {
        DEBUG_LOG("GNU support only OpenGL");
    }
    graphicsApi = std::make_unique<OpenGLApi::OpenGLApi>();
#endif

    AvatarGame::Start(std::move(graphicsApi));
    return 0;
}
