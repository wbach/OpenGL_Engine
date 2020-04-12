#include "Engine.h"
#include "Configuration.h"
#include "GameEngine/Display/DisplayManager.hpp"
#include "GraphicsApi/IGraphicsApi.h"
#include "Input/InputManager.h"
#include "Logger/Log.h"

namespace GameEngine
{
Engine::Engine(std::unique_ptr<GraphicsApi::IGraphicsApi> graphicsApi, std::unique_ptr<Physics::IPhysicsApi> physicsApi,
               SceneFactoryBasePtr sceneFactory)
    : graphicsApi_(std::move(graphicsApi))
    , physicsApi_(std::move(physicsApi))
    , displayManager_(*graphicsApi_, EngineConf.window.name, EngineConf.window.size.x, EngineConf.window.size.y,
          EngineConf.window.fullScreen ? GraphicsApi::WindowType::FULL_SCREEN : GraphicsApi::WindowType::WINDOW)
    , inputManager_(displayManager_.CreateInput())
    , renderersManager_(*graphicsApi_)
    , sceneManager_(*graphicsApi_, *physicsApi_, sceneFactory, displayManager_, *inputManager_,
                    renderersManager_, guiContext_, [&](EngineEvent e) { AddEngineEvent(e); })
    , introRenderer_(*graphicsApi_, displayManager_)
    , isRunning_(true)
{
    graphicsApi_->SetBackgroundColor(vec3(.8f));
    graphicsApi_->SetShadersFilesLocations(EngineConf.files.shaders);
    SetDisplay();
    sceneManager_.SetFactor();
}

Engine::~Engine()
{
    DEBUG_LOG("");
    sceneManager_.Reset();
    EngineConf_SaveRequiredFiles();
}

void Engine::SetDisplay()
{
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
    return displayManager_;
}

SceneManager& Engine::GetSceneManager()
{
    return sceneManager_;
}

void Engine::MainLoop()
{
    displayManager_.StartFrame();
    inputManager_->GetPressedKeys();
    displayManager_.ProcessEvents();
    sceneManager_.Update();

    sceneManager_.RuntimeGpuTasks();
    renderersManager_.RenderScene(sceneManager_.GetActiveScene(), displayManager_.GetTime());
    displayManager_.UpdateWindow();

    ProcessEngineEvents();
    displayManager_.EndFrame();
}

void Engine::ProcessEngineEvents()
{
    std::lock_guard<std::mutex> lk(engineEventsMutex);

    if (engineEvents.empty())
        return;

    auto event = engineEvents.front();
    engineEvents.pop_front();

    switch (event)
    {
        case EngineEvent::QUIT:
            Quit();
            break;
        default:
            break;
    }
}

void Engine::Quit()
{
    sceneManager_.Stop();
    isRunning_ = false;
}

void Engine::Init()
{
    graphicsApi_->EnableDepthTest();
    renderersManager_.Init();
    renderersManager_.GetDebugRenderer().SetPhysicsDebugDraw([this](const mat4& viewMatrix, const mat4& projectionMatrix) {
        physicsApi_->DebugDraw(viewMatrix, projectionMatrix);
    });
}
}  // namespace GameEngine
