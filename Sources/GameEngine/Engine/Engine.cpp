#include "Engine.h"

#include <GraphicsApi/IGraphicsApi.h>
#include <Input/InputManager.h>
#include <Logger/Log.h>

#include "Configuration.h"
#include "EngineContext.h"
#include "GameEngine/Display/DisplayManager.hpp"
#include "GameEngine/Components/RegisterReadFunctionForDefaultEngineComponents.h"

namespace GameEngine
{
namespace
{
const std::string FPS_ENGINE_CONTEXT{"RenderThreadFps"};
}

Engine::Engine(std::unique_ptr<GraphicsApi::IGraphicsApi> graphicsApi, std::unique_ptr<Physics::IPhysicsApi> physicsApi,
               std::unique_ptr<SceneFactoryBase> sceneFactory)
    : engineContext_(std::move(graphicsApi), std::move(physicsApi))
    , sceneManager_(engineContext_, std::move(sceneFactory))
    , introRenderer_(engineContext_.GetGraphicsApi(), engineContext_.GetGpuResourceLoader(),
                     engineContext_.GetDisplayManager())
    , isRunning_(true)
{
    Components::RegisterReadFunctionForDefaultEngineComponents();

    if (EngineConf.debugParams.logLvl != LogginLvl::None)
    {
        CLogger::Instance().EnableLogs(EngineConf.debugParams.logLvl);
        CLogger::Instance().ImmeditalyLog();
    }

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

    engineContext_.GetGraphicsApi().SetShadersFilesLocations(EngineConf.files.shaders);
    SetDisplay();
    sceneManager_.SetFactor();
}

Engine::~Engine()
{
    EngineConf.debugParams.logLvl.unsubscribe(loggingLvlParamSub_);
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

void Engine::SetDisplay()
{
    introRenderer_.Render();
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
    RuntimeGpuTasks();

    engineContext_.GetInputManager().GetPressedKeys();
    displayManager.ProcessEvents();
    sceneManager_.Update();
    engineContext_.GetGraphicsApi().PrepareFrame();
    auto scene = sceneManager_.GetActiveScene();
    if (scene)
    {
        engineContext_.GetRenderersManager().renderScene(*scene);
    }
    displayManager.UpdateWindow();

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
    //[&]() { return engineContext_.GetPhysicsApi().DebugDraw(); });
}

void Engine::RuntimeGpuTasks()
{
    RuntimeReleaseObjectGpu();
    RuntimeLoadObjectToGpu();
    RuntimeUpdateObjectGpu();
    RuntimeCallFunctionGpu();
}

void Engine::RuntimeLoadObjectToGpu()
{
    auto& gpuLoader = engineContext_.GetGpuResourceLoader();
    auto obj        = gpuLoader.GetObjectToGpuLoadingPass();

    while (obj)
    {
        if (not obj->GetGraphicsObjectId())
        {
            obj->GpuLoadingPass();
        }
        else
        {
            DEBUG_LOG("Is already loaded.");
        }

        obj = gpuLoader.GetObjectToGpuLoadingPass();
    }
}

void Engine::RuntimeUpdateObjectGpu()
{
    auto& gpuLoader = engineContext_.GetGpuResourceLoader();
    auto obj        = gpuLoader.GetObjectToUpdateGpuPass();

    while (obj)
    {
        if (obj->GetGraphicsObjectId())
        {
            obj->UpdateGpuPass();
        }
        else
        {
            ERROR_LOG("Object not loaded");
        }

        obj = gpuLoader.GetObjectToUpdateGpuPass();
    }
}

void Engine::RuntimeReleaseObjectGpu()
{
    auto& gpuLoader = engineContext_.GetGpuResourceLoader();
    auto obj        = gpuLoader.GetObjectToRelease();

    while (obj)
    {
        if (obj->GetGraphicsObjectId())
        {
            obj->ReleaseGpuPass();
        }
        obj = gpuLoader.GetObjectToRelease();
    }
}

void Engine::RuntimeCallFunctionGpu()
{
    auto& gpuLoader = engineContext_.GetGpuResourceLoader();
    gpuLoader.CallFunctions();
}
}  // namespace GameEngine
