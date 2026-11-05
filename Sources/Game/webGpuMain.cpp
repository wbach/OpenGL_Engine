#ifdef __EMSCRIPTEN__

#include <Logger/Log.h>
#include <emscripten.h>

#include <cstdio>
#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include "Game/StartupSceneHelpers.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Engine/Engine.h"
#include "GameEngine/Physics/Bullet/BulletAdapter.h"
#include "WebGpuApi/WebGpuApi.h"

using namespace GameEngine;
using namespace GameEngine::Physics;

namespace
{
// Silnik musi istnieć przez cały czas działania aplikacji.
std::unique_ptr<Engine> engine;

// Obiekty oczekujące na zakończenie inicjalizacji WebGPU.
std::unique_ptr<SceneFactoryBase> pendingSceneFactory;

std::unique_ptr<GraphicsApi::WebGpu::WebGpuApi> pendingWebGpuApi;

std::string pendingStartupScene;

// ---------------------------------------------------------
// Główna pętla silnika
// ---------------------------------------------------------

void EngineMainLoop(void*)
{
    if (pendingWebGpuApi)
        pendingWebGpuApi->ProcessEvents();

    // Silnik nie jest jeszcze gotowy — czekamy na WebGPU.
    if (!engine)
        return;

    engine->MainLoop();
}

// ---------------------------------------------------------
// Callback wywoływany po przygotowaniu WebGPU
// ---------------------------------------------------------

void OnWebGpuContextReady(bool success)
{
    if (!success)
    {
        LOG_ERROR << "WebGPU device initialization failed";
        return;
    }

    LOG_INFO << "WebGPU device is ready. Creating engine.";

    try
    {
        engine = std::make_unique<Engine>(std::make_unique<Bullet::BulletAdapter>(), std::move(pendingSceneFactory),
                                          std::move(pendingWebGpuApi));

        // Inicjalizacja silnika i rendererów.
        engine->Init();

        // Ustawiamy scenę startową.
        if (!pendingStartupScene.empty())
        {
            LOG_DEBUG << "Startup scene " << pendingStartupScene;

            engine->GetSceneManager().SetActiveScene(pendingStartupScene);
        }

        LOG_INFO << "Engine initialization completed.";
    }
    catch (const std::exception& e)
    {
        LOG_ERROR << "Engine initialization failed: " << e.what();
    }
}

}  // namespace

// ---------------------------------------------------------
// Main
// ---------------------------------------------------------

int main(int argc, char* argv[])
{
    std::puts("Aplikacja WebGPU uruchomiona.");

    CLogger::Instance().EnableLogs();
    CLogger::Instance().UseAsyncLogging(false);

    // -----------------------------------------------------
    // Wczytanie konfiguracji
    // -----------------------------------------------------

    const auto projectPath = Game::Startup::ResolveRuntimeProjectPath(argv[0]);
    EngineLocalConf.files.setProjectPath(projectPath);

    auto startupConfig = Game::Startup::LoadStartupSceneConfig(projectPath);
    if (!startupConfig.sceneFactory)
    {
        return 1;
    }

    pendingStartupScene = startupConfig.startupScene;

    // -----------------------------------------------------
    // Przygotowanie obiektów oczekujących na WebGPU
    // -----------------------------------------------------

    pendingSceneFactory = std::move(startupConfig.sceneFactory);

    pendingWebGpuApi = std::make_unique<GraphicsApi::WebGpu::WebGpuApi>();

    // -----------------------------------------------------
    // Rozpoczęcie inicjalizacji WebGPU
    // -----------------------------------------------------

    LOG_INFO << "Starting WebGPU initialization.";

    pendingWebGpuApi->CreateContextAsync(OnWebGpuContextReady);

    // -----------------------------------------------------
    // Uruchomienie pętli Emscripten
    // -----------------------------------------------------

    // Pętla może działać już teraz.
    // EngineMainLoop() nie wywoła MainLoop(), dopóki engine
    // nie zostanie utworzony w OnWebGpuContextReady().

    emscripten_set_main_loop_arg(EngineMainLoop, nullptr, 0, 0);

    return 0;
}

#endif