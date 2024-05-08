#include "Engine.h"

#include <GraphicsApi/IGraphicsApi.h>
#include <Input/InputManager.h>
#include <Logger/Log.h>

#include "Configuration.h"
#include "EngineContext.h"
#include "GameEngine/Components/RegisterReadFunctionForDefaultEngineComponents.h"
#include "GameEngine/Display/DisplayManager.hpp"

#ifndef USE_GNU
    #ifndef USE_MINGW // TO DO
    #include <DirectXApi/DirectXApi.h>
    #endif
#include <windows.h>
#include <shlobj.h>
#else
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#endif
#include <OpenGLApi/OpenGLApi.h>

namespace GameEngine
{
namespace
{
const std::string FPS_ENGINE_CONTEXT{"RenderThreadFps"};
}

ReadConfiguration::ReadConfiguration()
{
    std::string configFile("./Conf.xml");

#ifdef USE_GNU
    struct passwd* pw = getpwuid(getuid());
    configFile        = std::string(pw->pw_dir) + "/.config/bengine/Conf.xml";
#else
    wchar_t myDocumentsPath[1024];
    HRESULT hr = SHGetFolderPathW(0, CSIDL_MYDOCUMENTS, 0, 0, myDocumentsPath);
    if (SUCCEEDED(hr))
    {
        char str[1024];
        wcstombs(str, myDocumentsPath, 1023);
        configFile = std::string(str) + "\\bengine\\Conf.xml";
    }
#endif
    GameEngine::ReadFromFile(configFile);

    if (EngineConf.debugParams.logLvl != LogginLvl::None)
    {
        CLogger::Instance().EnableLogs(EngineConf.debugParams.logLvl);
        CLogger::Instance().ImmeditalyLog();
        std::cout << "LogginLvl: " << Params::paramToString(EngineConf.debugParams.logLvl) << std::endl;
    }
}

std::unique_ptr<GraphicsApi::IGraphicsApi> createGraphicsApi()
{
    std::unique_ptr<GraphicsApi::IGraphicsApi> graphicsApi;

#if !defined(USE_GNU) && !defined (USE_MINGW)
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
    graphicsApi->SetBackgroundColor(Color(0.18f, 0.27f, 0.47f));

    return graphicsApi;
}

Engine::Engine(std::unique_ptr<Physics::IPhysicsApi> physicsApi, std::unique_ptr<SceneFactoryBase> sceneFactory)
    : readConfiguration_()
    , engineContext_(createGraphicsApi(), std::move(physicsApi))
    , sceneManager_(engineContext_, std::move(sceneFactory))
    , introRenderer_(engineContext_.GetGraphicsApi(), engineContext_.GetGpuResourceLoader(),
                     engineContext_.GetDisplayManager())
    , isRunning_(true)
{
    srand((unsigned)time(NULL));
    Components::RegisterReadFunctionForDefaultEngineComponents();

    loggingLvlParamSub_ = EngineConf.debugParams.logLvl.subscribeForChange(
        []()
        {
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

    fpsLimitParamSub_ = EngineConf.renderer.fpsLimt.subscribeForChange(
        [this]()
        {
            auto physicsSubscriber = engineContext_.GetThreadSync().GetSubscriber(physicsThreadId_);
            if (physicsSubscriber)
                physicsSubscriber->SetFpsLimit(EngineConf.renderer.fpsLimt);
        });

    engineContext_.GetGraphicsApi().SetShadersFilesLocations(EngineConf.files.shaders);
    introRenderer_.Render();
    sceneManager_.SetFactor();

    engineContext_.GetPhysicsApi().DisableSimulation();
    physicsThreadId_ = engineContext_.GetThreadSync().Subscribe(
        [this](float deltaTime)
        {
            auto tm = engineContext_.GetDisplayManager().GetTimeMultiplayer();
            engineContext_.GetPhysicsApi().Simulate(deltaTime * tm);
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
                                                                          [this](bool ok)
                                                                          {
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
