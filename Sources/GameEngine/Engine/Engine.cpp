#include "Engine.h"
#include "Configuration.h"
#include "Logger/Log.h"

namespace GameEngine
{
Engine::Engine(IGraphicsApiPtr graphicsApi, Physics::IPhysicsApiPtr physicsApi, SceneFactoryBasePtr sceneFactory)
    : displayManager(nullptr)
    , inputManager_(nullptr)
    , renderersManager_(graphicsApi, shaderFactory_)
    , sceneManager_(graphicsApi, physicsApi, sceneFactory, displayManager, shaderFactory_, inputManager_,
                    renderersManager_,
                    guiContext_)
    , graphicsApi_(graphicsApi)
    , shaderFactory_(graphicsApi)
    , introRenderer_(graphicsApi, displayManager, shaderFactory_)
    , isRunning(true)
{
    graphicsApi_->SetBackgroundColor(vec3(.8f));

    SetDisplay();
    sceneManager_.SetFactor();
}

Engine::~Engine()
{
    sceneManager_.Reset();
    Log("");
    EngineConf_SaveRequiredFiles();
}

void Engine::SetDisplay()
{
    auto& conf = EngineConf;

    displayManager =
        std::make_shared<DisplayManager>(graphicsApi_, conf.window.name, conf.window.size.x, conf.window.size.y,
                                         conf.window.fullScreen ? WindowType::FULL_SCREEN : WindowType::WINDOW);
    inputManager_ = displayManager->CreateInput();
    introRenderer_.Render();
}

void Engine::GameLoop()
{
    while (isRunning.load())
        MainLoop();
}

void Engine::AddEngineEvent(EngineEvent event)
{
    std::lock_guard<std::mutex> lk(engineEventsMutex);
    engineEvents.push_back(event);
}

void Engine::Render()
{
    renderersManager_.RenderScene(sceneManager_.GetActiveScene());
}

DisplayManager& Engine::GetDisplayManager()
{
    return *displayManager;
}

void Engine::MainLoop()
{
    inputManager_->GetPressedKeys();
    sceneManager_.RuntimeLoadObjectToGpu();
    PrepareFrame();

    if (inputManager_->GetKey(KeyCodes::ESCAPE))
        isRunning.store(false);

    Render();
    sceneManager_.Update();

    ProcessEngineEvents();

    displayManager->Update();
}

void Engine::ProcessEngineEvents()
{
    EngineEvent event;
    {
        std::lock_guard<std::mutex> lk(engineEventsMutex);

        if (engineEvents.empty())
            return;

        event = engineEvents.front();
        engineEvents.pop_front();
    }

    switch (event)
    {
        case EngineEvent::LOAD_NEXT_SCENE:

            break;
        case EngineEvent::QUIT:
            break;
        default:
            break;
    }
}

void Engine::PrepareFrame()
{
    graphicsApi_->PrepareFrame();
    displayManager->ProcessEvents();
}

void Engine::Init()
{
    graphicsApi_->EnableDepthTest();
    renderersManager_.Init();
}
}  // GameEngine
