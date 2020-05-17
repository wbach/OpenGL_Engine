#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Engine/Engine.h"
#include "Logger/Log.h"
#include "OpenGLApi/OpenGLApi.h"

#ifndef USE_GNU
#include "DirectXApi/DirectXApi.h"
#endif

#include "Editor/Context.h"
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
    CLogger::Instance().ImmeditalyLog();

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
    Editor::Context editorContext;
    graphicsApi->SetBackgroundColor(Color(0.18f, 0.27f, 0.47f));
    GraphicsApi::IGraphicsApi& apiRef = *graphicsApi;
    Engine engine(std::move(graphicsApi), std::make_unique<BulletAdapter>(apiRef),
                  std::make_unique<Editor::SceneFactory>(editorContext));
    engine.Init();
    engine.GetSceneManager().SetActiveScene("EditorScene");
    engine.GameLoop();

    return 0;
}
