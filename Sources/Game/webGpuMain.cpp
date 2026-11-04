#ifdef __EMSCRIPTEN__

#include <Logger/Log.h>
#include <Utils/Json/JsonReader.h>
#include <Utils/Json/JsonWriter.h>
#include <emscripten.h>

#include <cstdio>
#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Engine/Engine.h"
#include "GameEngine/Physics/Bullet/BulletAdapter.h"
#include "GameEngine/Resources/File.h"
#include "GameEngine/Scene/SceneFactoryBase.h"
#include "GameEngine/Scene/SceneUtils.h"
#include "TreeNode.h"
#include "WebGpuApi/WebGpuApi.h"

using namespace GameEngine;
using namespace GameEngine::Physics;

namespace
{
const std::string CSTR_ROOT_NODE{"projectConfiguration"};
const std::string CSTR_STARTUP_SCENE_NODE{"startupScene"};
const std::string CSTR_SCENES_NODE{"scenes"};

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

    Utils::JsonReader jsonReader;

    const std::string inputFile = "/scenes.json";

    if (!jsonReader.Read(inputFile))
    {
        LOG_DEBUG << inputFile << " read failed";

        if (std::filesystem::exists(inputFile))
            return 1;

        GameEngine::createScenesFile(inputFile);
        return 1;
    }

    auto scenesNode = jsonReader.Get(CSTR_SCENES_NODE);

    PrintTree(*jsonReader.Get());

    if (!scenesNode)
    {
        LOG_ERROR << CSTR_SCENES_NODE << " not found in project config file";

        return 1;
    }

    // -----------------------------------------------------
    // Przygotowanie SceneFactory
    // -----------------------------------------------------

    auto sceneFactory = std::make_unique<SceneFactoryBase>();

    for (const auto& sceneNode : scenesNode->getChildren())
    {
        if (!sceneNode)
            continue;

        LOG_DEBUG << "Add scene " << sceneNode->name() << ", " << sceneNode->value_;

        sceneFactory->AddScene(sceneNode->name(), GameEngine::File{sceneNode->value_});
    }

    if (sceneFactory->ScenesSize() < 1)
    {
        LOG_ERROR << "Scene not available";
        return 1;
    }

    // -----------------------------------------------------
    // Odczyt sceny startowej
    // -----------------------------------------------------

    auto startupScene = jsonReader.Get(CSTR_STARTUP_SCENE_NODE);

    if (startupScene)
    {
        pendingStartupScene = startupScene->value_;
    }

    // -----------------------------------------------------
    // Przygotowanie obiektów oczekujących na WebGPU
    // -----------------------------------------------------

    pendingSceneFactory = std::move(sceneFactory);

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