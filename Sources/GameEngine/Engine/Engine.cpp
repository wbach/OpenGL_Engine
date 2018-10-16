#include "Engine.h"
#include "Configuration.h"
#include "Logger/Log.h"
#include "GameEngine/Engine/EngineMeasurement.h"

namespace GameEngine
{
CEngine::CEngine(IGraphicsApiPtr graphicsApi, Physics::IPhysicsApiPtr physicsApi, SceneFactoryBasePtr sceneFactory)
    : displayManager(nullptr)
    , inputManager_(nullptr)
    , renderersManager_(graphicsApi)
    , sceneManager_(graphicsApi, physicsApi, sceneFactory, displayManager, inputManager_, renderersManager_,
                    guiContext_)
    , graphicsApi_(graphicsApi)
    , introRenderer_(graphicsApi, displayManager)
    , isRunning(true)
{
    graphicsApi_->SetBackgroundColor(vec3(.8f));

    SetDisplay();
    sceneManager_.SetFactor();
}

CEngine::~CEngine()
{
    sceneManager_.Reset();
    Log("");
    EngineConf_SaveRequiredFiles();
}

void CEngine::SetDisplay()
{
    auto& conf = EngineConf;

    displayManager = std::make_shared<CDisplayManager>(
        graphicsApi_, conf.window.name, conf.window.size.x, conf.window.size.y,
        conf.window.fullScreen ? WindowType::FULL_SCREEN : WindowType::WINDOW);
    inputManager_ = displayManager->CreateInput();
    introRenderer_.Render();
}

void CEngine::GameLoop()
{
    while (isRunning.load())
        MainLoop();
}

void CEngine::AddEngineEvent(EngineEvent event)
{
    std::lock_guard<std::mutex> lk(engineEventsMutex);
    engineEvents.push_back(event);
}

void CEngine::Render()
{
    renderersManager_.RenderScene(sceneManager_.GetActiveScene());
}

CDisplayManager& CEngine::GetDisplayManager()
{
    return *displayManager;
}

void CEngine::MainLoop()
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

	WriteMeasurement("measurements.txt");
}

void CEngine::ProcessEngineEvents()
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

void CEngine::PrepareFrame()
{
    graphicsApi_->PrepareFrame();
    displayManager->ProcessEvents();
}

void CEngine::Init()
{
    graphicsApi_->EnableDepthTest();
    renderersManager_.Init();
}
}  // GameEngine
