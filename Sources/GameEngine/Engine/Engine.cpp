#include "Engine.h"

#include <GraphicsApi/IGraphicsApi.h>
#include <Input/InputManager.h>
#include <Logger/Log.h>
#include <Utils/Variant.h>

#include <memory>

#include "Configuration.h"
#include "EngineContext.h"
#include "GameEngine/Components/RegisterReadFunctionForDefaultEngineComponents.h"
#include "GameEngine/Display/DisplayManager.hpp"
#include "GameEngine/Physics/IPhysicsApi.h"
#include "GameEngine/Resources/IResourceManagerFactory.h"

#ifndef USE_GNU
#ifndef USE_MINGW  // TO DO
#include <DirectXApi/DirectXApi.h>
#endif
#include <shlobj.h>
#include <windows.h>
#else
#endif
#include <OpenGLApi/OpenGLApi.h>
#include <signal.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>

#ifdef USE_GNU
#include <execinfo.h>

void bt_sighandler(int nSig)
{
    LOG_ERROR << "print_trace: got signal " << nSig;

    void* array[32];
    size_t size;
    char** strings;
    size_t nCnt;

    size = backtrace(array, 32);

    strings = backtrace_symbols(array, size);

    for (nCnt = 0; nCnt < size; nCnt++)
        LOG_ERROR << strings[nCnt];

    exit(-1);
}
#else
#include <dbghelp.h>

#include <string>

#pragma comment(lib, "dbghelp.lib")

void bt_sighandler(int nSig)
{
    /* LOG TO FIX*/ LOG_ERROR << ("print_trace: got signal " + std::to_string(nSig));

    HANDLE hProcess = GetCurrentProcess();
    SymInitialize(hProcess, NULL, TRUE);

    void* stack[32];
    USHORT frames = CaptureStackBackTrace(0, 32, stack, NULL);

    for (USHORT i = 0; i < frames; ++i)
    {
        DWORD64 addr = (DWORD64)(stack[i]);

        char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(char)];
        PSYMBOL_INFO symbol  = reinterpret_cast<PSYMBOL_INFO>(symbolBuffer);
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen   = MAX_SYM_NAME;

        DWORD64 displacement = 0;
        if (SymFromAddr(hProcess, addr, &displacement, symbol))
        {
            /* LOG TO FIX*/ LOG_ERROR << (std::string(symbol->Name) + " [0x" + std::to_string(symbol->Address) + "]");
        }
        else
        {
            /* LOG TO FIX*/ LOG_ERROR << ("Unknown function at [0x" + std::to_string((uintptr_t)addr) + "]");
        }
    }

    SymCleanup(hProcess);

    signal(nSig, SIG_DFL);
    raise(nSig);
}
#endif

namespace GameEngine
{
namespace
{
std::unique_ptr<GraphicsApi::IGraphicsApi> createGraphicsApi()
{
    std::unique_ptr<GraphicsApi::IGraphicsApi> graphicsApi;

#if !defined(USE_GNU) && !defined(USE_MINGW)
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
        LOG_ERROR << "GNU support only OpenGL";
    }
    graphicsApi = std::make_unique<OpenGLApi::OpenGLApi>();
#endif
    graphicsApi->SetBackgroundColor(Color(0.18f, 0.27f, 0.47f));

    return graphicsApi;
}
}  // namespace

Engine::Engine(std::unique_ptr<Physics::IPhysicsApi> physicsApi, std::unique_ptr<ISceneFactory> sceneFactory,
               std::unique_ptr<GraphicsApi::IGraphicsApi> graphicsApi)
    : readConfiguration_()
    , engineContext_(graphicsApi ? std::move(graphicsApi) : createGraphicsApi(), std::move(physicsApi), std::move(sceneFactory))
    , externalComponents_(engineContext_.GetSceneManager())
    , introRenderer_(engineContext_.GetGraphicsApi(), engineContext_.GetGpuResourceLoader(), engineContext_.GetDisplayManager(),
                     engineContext_.GetResourceManagerFactory())
    , isRunning_(true)
{
    LOG_DEBUG << "Start engine.";

    signal(SIGSEGV, bt_sighandler);
    srand((unsigned)time(NULL));
    Components::RegisterReadFunctionForDefaultEngineComponents();

    loggingLvlParamSub_ = EngineConf.debugParams.logLvl.subscribeForChange(
        []()
        {
            if (EngineConf.debugParams.logLvl != LoggingLvl::None)
            {
                CLogger::Instance().EnableLogs(EngineConf.debugParams.logLvl);
                CLogger::Instance().UseAsyncLogging(false);
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

    engineContext_.GetPhysicsApi().DisableSimulation();
    physicsThreadId_ = engineContext_.GetThreadSync().Subscribe(
        [this](float deltaTime)
        {
            auto tm = engineContext_.GetDisplayManager().GetTimeMultiplayer();
            engineContext_.GetPhysicsApi().Simulate(deltaTime * tm);
        },
        "Physics", EngineConf.renderer.fpsLimt);

    showPhycicsVisualizationSub_ = EngineConf.debugParams.showPhycicsVisualization.subscribeForChange(
        [this]()
        {
            if (EngineConf.debugParams.showPhycicsVisualization)
            {
                engineContext_.GetPhysicsApi().enableVisualizationForAllRigidbodys();
            }
            else
            {
                engineContext_.GetPhysicsApi().disableVisualizationForAllRigidbodys();
            }
        });

    quitApiSubId_ = engineContext_.GetGraphicsApi().GetWindowApi().SubscribeForEvent(
        [&](const auto& event) {
            std::visit(visitor{[&](const GraphicsApi::QuitEvent&) { Quit(); }, [](const GraphicsApi::DropFileEvent&) {}}, event);
        });
}

Engine::~Engine()
{
    LOG_DEBUG << "destructor";
    EngineConf.debugParams.showPhycicsVisualization.unsubscribe(showPhycicsVisualizationSub_);
    EngineConf.debugParams.logLvl.unsubscribe(loggingLvlParamSub_);
    EngineConf.renderer.fpsLimt.unsubscribe(fpsLimitParamSub_);
    EngineConf_SaveRequiredFiles();
    LOG_DEBUG << "destructor done";
}

void Engine::CheckThreadsBeforeQuit()
{
    if (engineContext_.GetThreadSync().SubscribersCount() > 0)
    {
        LOG_ERROR << "Not closed threads. Force to close.";
        engineContext_.GetThreadSync().Stop();
    }
}

ExternalComponentsReader& Engine::getExternalComponentsReader()
{
    return externalComponents_;
}

void Engine::GameLoop()
{
    while (isRunning_)
    {
        MainLoop();
    }

    CheckThreadsBeforeQuit();

    engineContext_.GetGraphicsApi().GetWindowApi().UnsubscribeForEvent(quitApiSubId_);
    engineContext_.GetThreadSync().Unsubscribe(physicsThreadId_);
    engineContext_.GetSceneManager().Reset();
}

ISceneManager& Engine::GetSceneManager()
{
    return engineContext_.GetSceneManager();
}

EngineContext& Engine::GetEngineContext()
{
    return engineContext_;
}

void Engine::MainLoop()
{
    auto& displayManager = engineContext_.GetDisplayManager();

    displayManager.StartFrame();
    engineContext_.GetGpuResourceLoader().RuntimeGpuTasks();

    engineContext_.GetInputManager().GetPressedKeys();
    displayManager.ProcessEvents();
    engineContext_.GetSceneManager().Update();
    engineContext_.GetGraphicsApi().PrepareFrame();
    auto scene = engineContext_.GetSceneManager().GetActiveScene();
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
    auto events = engineContext_.GetEngineEvents();

    for (auto& event : events)
    {
        std::visit(visitor{[&](QuitEvent& e) { Quit(e); },
                           [&](ChangeSceneEvent& e) { engineContext_.GetSceneManager().ProcessEvent(e); },
                           [&](ChangeSceneConfirmEvent& e) { engineContext_.GetSceneManager().ProcessEvent(e); }},
                   event);
    }
}

void Engine::Quit()
{
    engineContext_.GetSceneManager().StopThread();
    isRunning_ = false;
}

void Engine::Quit(QuitEvent event)
{
    switch (event)
    {
        case QuitEvent::QUIT:
            Quit();
            break;
        case QuitEvent::ASK_QUIT:
            auto& windowApi = engineContext_.GetGraphicsApi().GetWindowApi();
            windowApi.ShowMessageBox("Quit", "Do you really want exit?",
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

void Engine::Init()
{
    engineContext_.GetGraphicsApi().EnableDepthTest();
    engineContext_.GetRenderersManager().Init();
    engineContext_.GetRenderersManager().GetDebugRenderer().SetPhysicsDebugDraw(
        std::bind(&Physics::IPhysicsApi::DebugDraw, &engineContext_.GetPhysicsApi()));
}
}  // namespace GameEngine
