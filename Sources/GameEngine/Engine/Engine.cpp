#include "Engine.h"

#include <GraphicsApi/IGraphicsApi.h>
#include <Input/InputManager.h>
#include <Logger/Log.h>
#include <Utils/Variant.h>

#include "Configuration.h"
#include "EngineContext.h"
#include "GameEngine/Components/RegisterReadFunctionForDefaultEngineComponents.h"
#include "GameEngine/Display/DisplayManager.hpp"

#ifndef USE_GNU
#ifndef USE_MINGW  // TO DO
#include <DirectXApi/DirectXApi.h>
#endif
#include <shlobj.h>
#include <windows.h>
#else
#include <dlfcn.h>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#endif
#include <OpenGLApi/OpenGLApi.h>
#include <signal.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>

#ifdef USE_GNU
#include <execinfo.h>

void bt_sighandler(int nSig)
{
    ERROR_LOG("print_trace: got signal " + std::to_string(nSig));

    void* array[32];
    size_t size;
    char** strings;
    size_t nCnt;

    size = backtrace(array, 32);

    strings = backtrace_symbols(array, size);

    for (nCnt = 0; nCnt < size; nCnt++)
        ERROR_LOG(strings[nCnt]);

    exit(-1);
}
#else
#include <dbghelp.h>

#include <string>

#pragma comment(lib, "dbghelp.lib")

void bt_sighandler(int nSig)
{
    ERROR_LOG("print_trace: got signal " + std::to_string(nSig));

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
            ERROR_LOG(std::string(symbol->Name) + " [0x" + std::to_string(symbol->Address) + "]");
        }
        else
        {
            ERROR_LOG("Unknown function at [0x" + std::to_string((uintptr_t)addr) + "]");
        }
    }

    SymCleanup(hProcess);

    signal(nSig, SIG_DFL);
    raise(nSig);
}
#endif

typedef void (*registerReadFunction)();

namespace GameEngine
{
namespace
{
const std::string FPS_ENGINE_CONTEXT{"RenderThreadFps"};

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
        DEBUG_LOG("GNU support only OpenGL");
    }
    graphicsApi = std::make_unique<OpenGLApi::OpenGLApi>();
#endif
    graphicsApi->SetBackgroundColor(Color(0.18f, 0.27f, 0.47f));

    return graphicsApi;
}
}  // namespace

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

ExternalComponents::ExternalComponents()
{
    LOG_DEBUG << "Check for ExternalComponents";
    std::string libExtension{".so"};
    const auto componentsDir = EngineConf.files.data + "/Components";
    if (not Utils::DirectoryExist(componentsDir))
    {
        LOG_DEBUG << "Components dir not exist : " << componentsDir;
        return;
    }

    const auto& files = Utils::FindFilesWithExtension(componentsDir, libExtension);
    LOG_DEBUG << "Found files : " << files.size();

    for (const auto& file : files)
    {
        LOG_DEBUG << "Component file detected : " << file;

#ifdef USE_GNU
        void* componentLib = dlopen(file.c_str(), RTLD_NOW);
#else
        HMODULE sceneLib = LoadLibrary(file.c_str());
#endif
        if (componentLib)
        {
            LOG_DEBUG << "Component : " << file << " lib opened. Search registerReadFunction";
#ifdef USE_GNU
            auto func = (registerReadFunction)dlsym(componentLib, "registerReadFunction");

#else
            auto func = (registerReadFunction)GetProcAddress(componentLib, "registerReadFunction");
#endif
            if (func)
            {
                LOG_DEBUG << "Component : " << file << " register function found. Call function. End loading.";
                func();
            }
        }
        else
        {
            LOG_WARN << "Open lib " << file << " failed : " << dlerror();
        }
    }
}

Engine::Engine(std::unique_ptr<Physics::IPhysicsApi> physicsApi, std::unique_ptr<ISceneFactory> sceneFactory,
               std::unique_ptr<GraphicsApi::IGraphicsApi> graphicsApi)
    : readConfiguration_()
    , externalComponents_()
    , engineContext_(graphicsApi ? std::move(graphicsApi) : createGraphicsApi(), std::move(physicsApi), std::move(sceneFactory))
    , introRenderer_(engineContext_.GetGraphicsApi(), engineContext_.GetGpuResourceLoader(), engineContext_.GetDisplayManager())
    , isRunning_(true)
{
    DEBUG_LOG("Start engine.");

    signal(SIGSEGV, bt_sighandler);
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
    engineContext_.GetGraphicsApi().GetWindowApi().UnsubscribeForEvent(quitApiSubId_);
    EngineConf.debugParams.showPhycicsVisualization.unsubscribe(showPhycicsVisualizationSub_);
    engineContext_.GetThreadSync().Unsubscribe(physicsThreadId_);
    EngineConf.debugParams.logLvl.unsubscribe(loggingLvlParamSub_);
    EngineConf.renderer.fpsLimt.unsubscribe(fpsLimitParamSub_);
    DEBUG_LOG("destructor");
    engineContext_.GetSceneManager().Reset();
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
    engineContext_.GetSceneManager().StopThread();
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
