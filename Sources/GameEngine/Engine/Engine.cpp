#include "Engine.h"
#include "Configuration.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "GameEngine/Display/DisplayManager.hpp"
#include "Input/InputManager.h"
#include "Logger/Log.h"

namespace GameEngine
{
Engine::Engine(std::unique_ptr<GraphicsApi::IGraphicsApi> graphicsApi, std::unique_ptr<Physics::IPhysicsApi> physicsApi,
               SceneFactoryBasePtr sceneFactory)
    : graphicsApi_(std::move(graphicsApi))
    , physicsApi_(std::move(physicsApi))
    , displayManager(nullptr)
    , inputManager_(nullptr)
    , renderersManager_(*graphicsApi_, shaderFactory_)
    , sceneManager_(*graphicsApi_, *physicsApi_, sceneFactory, displayManager, shaderFactory_, inputManager_,
                    renderersManager_, guiContext_)
    , shaderFactory_(*graphicsApi_)
    , introRenderer_(*graphicsApi_, displayManager, shaderFactory_)
    , isRunning_(true)
{
    graphicsApi_->SetBackgroundColor(vec3(.8f));
    renderersManager_.SetPhysicsDebugDraw([this](const mat4& viewMatrix, const mat4& projectionMatrix) {
        physicsApi_->DebugDraw(viewMatrix, projectionMatrix);
    });
    graphicsApi_->SetShadersFilesLocations(EngineConf.files.shaders);
    SetDisplay();
    sceneManager_.SetFactor();
}

Engine::~Engine()
{
    Log(__FUNCTION__);
    sceneManager_.Reset();
    EngineConf_SaveRequiredFiles();
}

void Engine::SetDisplay()
{
    auto& conf = EngineConf;

    displayManager = std::make_shared<DisplayManager>(
        *graphicsApi_, conf.window.name, conf.window.size.x, conf.window.size.y,
        conf.window.fullScreen ? GraphicsApi::WindowType::FULL_SCREEN : GraphicsApi::WindowType::WINDOW);
    inputManager_ = displayManager->CreateInput();
    introRenderer_.Render();
}

void Engine::GameLoop()
{
    while (isRunning_)
        MainLoop();
}

void Engine::AddEngineEvent(EngineEvent event)
{
    std::lock_guard<std::mutex> lk(engineEventsMutex);
    engineEvents.push_back(event);
}

DisplayManager& Engine::GetDisplayManager()
{
    return *displayManager;
}

SceneManager& Engine::GetSceneManager()
{
    return sceneManager_;
}

void Engine::MainLoop()
{
    inputManager_->GetPressedKeys();
    if (inputManager_->GetKey(KeyCodes::ESCAPE))
    {
        sceneManager_.Stop();
        isRunning_ = false;
        return;
    }

    sceneManager_.RuntimeLoadObjectToGpu();
    PrepareFrame();
    renderersManager_.RenderScene(sceneManager_.GetActiveScene(), displayManager->GetTime());
    sceneManager_.Update();
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
}  // namespace GameEngine
