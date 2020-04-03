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

   // graphicsApi = std::make_unique<OpenGLApi::OpenGLApi>();
     graphicsApi = std::make_unique<DirectX::DirectXApi>();

    Editor::Context editorContext;

    GraphicsApi::IGraphicsApi& apiRef = *graphicsApi;
    Engine engine(std::move(graphicsApi), std::make_unique<BulletAdapter>(apiRef),
                  std::make_shared<Editor::SceneFactory>(editorContext));
    engine.Init();
    engine.GetSceneManager().SetActiveScene("EditorScene");
    engine.GameLoop();

    return 0;
}
