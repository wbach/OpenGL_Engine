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

    //vec3 vc(10, 1, 10);
    //vec3 z0(vc.x - 1, 1, vc.z - 1);
    //vec3 z3(vc.x - 1, 1, 0);

    //auto v1 = z0 - vc;
    //auto v2 = z3 - vc;


    //DEBUG_LOG(std::to_string(glm::normalize(glm::cross(v2, v1))));

    //vec3 ab = vc - z0;
    //vec3 cb = z3 - vc;
    //DEBUG_LOG(std::to_string(glm::normalize(glm::cross(ab, cb))));


   // return 0;

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

    GraphicsApi::IGraphicsApi& apiRef = *graphicsApi;
    Engine engine(std::move(graphicsApi), std::make_unique<BulletAdapter>(apiRef),
                  std::make_unique<Editor::SceneFactory>(editorContext));
    engine.Init();
    engine.GetSceneManager().SetActiveScene("EditorScene");
    engine.GameLoop();

    return 0;
}
