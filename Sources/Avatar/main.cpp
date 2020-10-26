#include <Utils/FileSystem/FileSystemUtils.hpp>

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Engine/Engine.h"
#include "Logger/Log.h"
#include <OpenGLApi/OpenGLApi.h>
#include <SdlOpenGL/SDLOpenGL.h>
#include "StartGame.h"
#ifndef USE_GNU
#include <DirectXApi/DirectXApi.h>
#endif

#include <GLM/GLMUtils.h>

const std::string configFile = "./Conf.xml";

int main(int, char**)
{
    CLogger::Instance().EnableLogs(LogginLvl::ErrorWarningInfoDebug);
    CLogger::Instance().ImmeditalyLog();

    GameEngine::ReadFromFile(configFile);
    std::unique_ptr<GraphicsApi::IGraphicsApi> graphicsApi;

#ifndef USE_GNU
    if (EngineConf.renderer.graphicsApi == "OpenGL")
    {
        graphicsApi = std::make_unique<OpenGLApi::OpenGLApi>(std::make_unique<OpenGLApi::SdlOpenGlApi>());
    }
    else if (EngineConf.renderer.graphicsApi == "DirectX11")
    {
        graphicsApi = std::make_unique<DirectX::DirectXApi>();
    }
    else
    {
        graphicsApi = std::make_unique<OpenGLApi::OpenGLApi>(std::make_unique<OpenGLApi::SdlOpenGlApi>());
    }
#else
    if (EngineConf.renderer.graphicsApi != "OpenGL")
    {
        DEBUG_LOG("GNU support only OpenGL");
    }
    graphicsApi = std::make_unique<OpenGLApi::OpenGLApi>();
#endif
    graphicsApi->SetBackgroundColor(Color(0.18f, 0.27f, 0.47f));
    AvatarGame::Start(std::move(graphicsApi));
    return 0;
}
