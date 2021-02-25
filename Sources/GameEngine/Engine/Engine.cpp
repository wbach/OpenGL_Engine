#include "Engine.h"

#include <GraphicsApi/IGraphicsApi.h>
#include <Input/InputManager.h>
#include <Logger/Log.h>

#include "Configuration.h"
#include "EngineContext.h"
#include "GameEngine/Components/RegisterReadFunctionForDefaultEngineComponents.h"
#include "GameEngine/Display/DisplayManager.hpp"

namespace GameEngine
{
namespace
{
const std::string FPS_ENGINE_CONTEXT{"RenderThreadFps"};
}

EnableLogger::EnableLogger()
{
    if (EngineConf.debugParams.logLvl != LogginLvl::None)
    {
        CLogger::Instance().EnableLogs(EngineConf.debugParams.logLvl);
        CLogger::Instance().ImmeditalyLog();
    }
}

Engine::Engine(std::unique_ptr<GraphicsApi::IGraphicsApi> graphicsApi, std::unique_ptr<Physics::IPhysicsApi> physicsApi,
               std::unique_ptr<SceneFactoryBase> sceneFactory)
    : enableLogger_()
    , engineContext_(std::move(graphicsApi), std::move(physicsApi))
    , sceneManager_(engineContext_, std::move(sceneFactory))
    , introRenderer_(engineContext_.GetGraphicsApi(), engineContext_.GetGpuResourceLoader(),
                     engineContext_.GetDisplayManager())
    , isRunning_(true)
{
    srand((unsigned)time(NULL));
    Components::RegisterReadFunctionForDefaultEngineComponents();

    loggingLvlParamSub_ = EngineConf.debugParams.logLvl.subscribeForChange([](auto&) {
        if (EngineConf.debugParams.logLvl != LogginLvl::None)
        {
            CLogger::Instance().EnableLogs(EngineConf.debugParams.logLvl);
            CLogger::Instance().ImmeditalyLog();
        }
        else
        {
            CLogger::Instance().DisableLogs();
        }
    });

    fpsLimitParamSub_ = EngineConf.renderer.fpsLimt.subscribeForChange([this](float newFpsLimit) {
        auto physicsSubscriber = engineContext_.GetThreadSync().GetSubscriber(physicsThreadId_);
        if (physicsSubscriber)
            physicsSubscriber->SetFpsLimit(newFpsLimit);
    });

    engineContext_.GetGraphicsApi().SetShadersFilesLocations(EngineConf.files.shaders);
    introRenderer_.Render();
    sceneManager_.SetFactor();

    physicsThreadId_ = engineContext_.GetThreadSync().Subscribe(
        [this](float deltaTime) {
            engineContext_.GetPhysicsApi().SetSimulationStep(deltaTime);
            engineContext_.GetPhysicsApi().Simulate();
        },
        "Physics", EngineConf.renderer.fpsLimt);
}

Engine::~Engine()
{
    engineContext_.GetThreadSync().Unsubscribe(physicsThreadId_);
    EngineConf.debugParams.logLvl.unsubscribe(loggingLvlParamSub_);
    EngineConf.renderer.fpsLimt.unsubscribe(fpsLimitParamSub_);
    DEBUG_LOG("destructor");
    sceneManager_.Reset();
    EngineConf_SaveRequiredFiles();
}

void Engine::CheckThreadsBeforeQuit()
{
    if (engineContext_.GetThreadSync().SubscribersCount() > 0)
    {
        WARNING_LOG("Not closed threads. Force to close.");
        engineContext_.GetThreadSync().Stop();
    }
}

void Engine::GameLoop()
{
    while (isRunning_)
    {
        MainLoop();
    }

    CheckThreadsBeforeQuit();
}

SceneManager& Engine::GetSceneManager()
{
    return sceneManager_;
}

void Engine::MainLoop()
{
    auto& displayManager = engineContext_.GetDisplayManager();

    displayManager.StartFrame();
    engineContext_.GetGpuResourceLoader().RuntimeGpuTasks();

    engineContext_.GetInputManager().GetPressedKeys();
    displayManager.ProcessEvents();
    sceneManager_.Update();
    engineContext_.GetGraphicsApi().PrepareFrame();
    auto scene = sceneManager_.GetActiveScene();
    if (scene)
    {
        engineContext_.GetRenderersManager().renderScene(*scene);
        displayManager.UpdateWindow();
    }

    ProcessEngineEvents();
    displayManager.EndFrame();
}

void Engine::ProcessEngineEvents()
{
    auto incomingEvent = engineContext_.GetEngineEvent();

    if (not incomingEvent)
        return;

    switch (*incomingEvent)
    {
        case EngineEvent::QUIT:
            Quit();
            break;
        case EngineEvent::ASK_QUIT:
            engineContext_.GetGraphicsApi().GetWindowApi().ShowMessageBox("Quit", "Do you really want exit?",
                                                                          [this](bool ok) {
                                                                              if (ok)
                                                                              {
                                                                                  Quit();
                                                                              }
                                                                          });
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
    engineContext_.GetGraphicsApi().EnableDepthTest();
    engineContext_.GetRenderersManager().Init();
    engineContext_.GetRenderersManager().GetDebugRenderer().SetPhysicsDebugDraw(
        std::bind(&Physics::IPhysicsApi::DebugDraw, &engineContext_.GetPhysicsApi()));
}

}  // namespace GameEngine
