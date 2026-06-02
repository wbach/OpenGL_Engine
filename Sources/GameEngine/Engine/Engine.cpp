#include "Engine.h"

#include <GraphicsApi/IGraphicsApi.h>
#include <Input/InputManager.h>
#include <Logger/Log.h>
#include <Utils/IThreadSync.h>
#include <Utils/ThreadSubscriber.h>
#include <Utils/Variant.h>

#include <memory>
#include <sstream>
#include <utility>

#include "Configuration.h"
#include "EngineContext.h"
#include "GameEngine/Audio/AudioManager.h"
#include "GameEngine/Display/DisplayManager.hpp"
#include "GameEngine/Engine/EngineEvent.h"
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
    std::stringstream out;
    out << "print_trace: got signal " << nSig << '\n';

    void* array[32];
    auto size    = backtrace(array, 32);
    auto strings = backtrace_symbols(array, size);

    if (strings == nullptr)
    {
        exit(-1);
    }

    for (auto nCnt = 0; nCnt < size; nCnt++)
    {
        std::string entry = strings[nCnt];

        auto begin_parentheses = entry.find_first_of('(');
        auto end_parentheses   = entry.find_first_of('+', begin_parentheses);

        if (begin_parentheses != std::string::npos and end_parentheses != std::string::npos)
        {
            auto mangled_name = entry.substr(begin_parentheses + 1, end_parentheses - begin_parentheses - 1);

            int status          = 0;
            auto demangled_name = abi::__cxa_demangle(mangled_name.c_str(), nullptr, nullptr, &status);

            if (status == 0 and demangled_name != nullptr)
            {
                out << entry.substr(0, begin_parentheses + 1) << demangled_name << entry.substr(end_parentheses) << '\n';
                free(demangled_name);
            }
            else
            {
                out << entry << '\n';
            }
        }
        else
        {
            out << entry << '\n';
        }
    }

    LOG_ERROR_RAW << out.str();

    free(strings);
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
std::unique_ptr<GraphicsApi::IGraphicsApi> createGraphicsApi(std::unique_ptr<GraphicsApi::IGraphicsApi> requestedGraphicsApi)
{
    if (requestedGraphicsApi)
    {
            LOG_DEBUG << "path " << EngineLocalConf.files.getShaderPath();
        requestedGraphicsApi->SetShadersFilesLocations(EngineLocalConf.files.getShaderPath());
        requestedGraphicsApi->DebugNormalMeshGeneration(EngineConf.debugParams.generateDebugNormalsMeshes);
        return requestedGraphicsApi;
    }

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

    LOG_DEBUG << "path " << EngineLocalConf.files.getShaderPath();
    graphicsApi->SetShadersFilesLocations(EngineLocalConf.files.getShaderPath());
    graphicsApi->DebugNormalMeshGeneration(EngineConf.debugParams.generateDebugNormalsMeshes);
    return graphicsApi;
}
}  // namespace

Engine::ConfigurationReader::ConfigurationReader()
{
    ReadConfigFromFile(EngineConf);
}

Engine::Engine(std::unique_ptr<Physics::IPhysicsApi> physicsApi, std::unique_ptr<ISceneFactory> sceneFactory,
               std::unique_ptr<GraphicsApi::IGraphicsApi> graphicsApi)
    : readConfiguration_()
    , engineContext_(createGraphicsApi(std::move(graphicsApi)), std::move(physicsApi), std::move(sceneFactory))
    , externalComponents_(engineContext_.GetSceneManager())
    , isRunning_(true)
{
    LOG_DEBUG << "Start engine.";

    signal(SIGSEGV, bt_sighandler);
    srand((unsigned)time(NULL));

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

void Engine::CleanUp()
{
    CheckThreadsBeforeQuit();

    engineContext_.GetGraphicsApi().GetWindowApi().UnsubscribeForEvent(quitApiSubId_);
    engineContext_.GetThreadSync().Unsubscribe(physicsThreadId_);
    LOG_DEBUG_RAW << "Reset scene";
    engineContext_.GetSceneManager().Reset();
    engineContext_.GetGpuResourceLoader().RuntimeGpuTasks();
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

    CleanUp();
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
    engineContext_.GetAudioManager().update();

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
        std::visit(visitor{[&](const SetGameStateFlag& e)
                           {
                               engineContext_.GetGameState().setFlag(e.flag, e.value);
                               engineContext_.GetQuestManager().onSetFlag(e.flag, e.value);
                           },
                           [&](const SceneStartedEvent&) { engineContext_.GetQuestManager().onSceneStarted(); },
                           [&](const QuitEvent& e) { Quit(e); },
                           [&](const ChangeSceneEvent& e) { engineContext_.GetSceneManager().ProcessEvent(e); },
                           [&](const ChangeSceneConfirmEvent& e) { engineContext_.GetSceneManager().ProcessEvent(e); }},
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
}
}  // namespace GameEngine
