#include "SceneLoader.h"

#include <Logger/Log.h>
#include <Types.h>
#include <Utils.h>
#include <Utils/Time/TimeMeasurer.h>
#include <Utils/Time/Timer.h>

#include <atomic>
#include <memory>
#include <optional>
#include <thread>

#include "GameEngine/Display/DisplayManager.hpp"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/LoadingScreenRenderer.h"
#include "GameEngine/Resources/IGpuResourceLoader.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/IResourceManagerFactory.h"
#include "GameEngine/Resources/ITextureLoader.h"
#include "ISceneFactory.h"
#include "Scene.hpp"

namespace GameEngine
{
SceneLoader::SceneLoader(ISceneFactory& factory, GraphicsApi::IGraphicsApi& graphicsApi, IGpuResourceLoader& gpuResourceLoader,
                         DisplayManager& displayManager, IResourceManagerFactory& resourceManagerFactory)
    : sceneFactory_{factory}
    , graphicsApi_(graphicsApi)
    , displayManager_(displayManager)
    , isReading(true)
    , objectCount_(0)
    , loadingScreenRenderer(nullptr)
    , resourceManager(resourceManagerFactory.create())
    , gpuLoader_(gpuResourceLoader)
    , bgTexture_(nullptr)
    , circleTexture_(nullptr)
{
    LOG_DEBUG << "SceneLoader::SceneLoader " << Utils::BoolToString(resourceManager != nullptr);
}

SceneLoader::~SceneLoader()
{
    LOG_DEBUG << "~SceneLoader";
    CleanUp();
}

void SceneLoader::CleanUp()
{
    if (loadingScreenRenderer)
    {
        loadingScreenRenderer->cleanUp();
        loadingScreenRenderer.reset();
    }

    if (resourceManager)
    {
        if (bgTexture_)
            resourceManager->GetTextureLoader().DeleteTexture(*bgTexture_);
        if (circleTexture_)
            resourceManager->GetTextureLoader().DeleteTexture(*circleTexture_);
    }

    bgTexture_     = nullptr;
    circleTexture_ = nullptr;
}

std::unique_ptr<Scene> SceneLoader::Load(uint32 id)
{
    Init();
    isReading.store(true);

    LOG_DEBUG << "Load scene :" << id;

    std::unique_ptr<Scene> scene;
    std::thread loadingThread([&]() { scene = LoadScene(id); });

    ScreenRenderLoop();
    loadingThread.join();

    return scene;
}

std::unique_ptr<Scene> SceneLoader::Load(const std::string& name)
{
    Init();
    isReading.store(true);

    LOG_DEBUG << "Load scene :" << name;
    std::unique_ptr<Scene> result;
    std::thread loadingThread([&]() { result = LoadScene(name); });
    LOG_DEBUG << "loadingThread done: " << name;
    ScreenRenderLoop();
    loadingThread.join();

    return result;
}

template <typename T>
std::unique_ptr<Scene> SceneLoader::LoadScene(T t)
{
    LOG_DEBUG << "Load scene thread started.";
    Utils::Timer timer;
    auto newScene = sceneFactory_.Create(t);
    newScene->Init();
    LOG_DEBUG << "Scene loading time: " << timer.GetTimeMiliSeconds() << "ms.";

    do
    {
        newScene->ProcessEvents();
    } while (IsGpuLoading());

    isReading.store(false);
    return newScene;
}

void SceneLoader::Init()
{
    LOG_DEBUG << "Init LoadingScreenRenderer";
    TextureParameters params;
    params.loadType        = TextureLoadType::Immediately;
    params.flipMode        = TextureFlip::VERTICAL;
    params.sizeLimitPolicy = SizeLimitPolicy::NoLimited;
    params.filter          = GraphicsApi::TextureFilter::LINEAR;
    params.mimap           = GraphicsApi::TextureMipmap::LINEAR;

    if (not resourceManager)
    {
        LOG_ERROR << "resourceManager not set!";
        return;
    }
    auto& texureLoader = resourceManager->GetTextureLoader();
    circleTexture_     = texureLoader.LoadTexture(EngineConf.files.loadingScreenCircleTexture, params);
    bgTexture_         = texureLoader.LoadTexture(EngineConf.files.loadingScreenBackgroundTexture, params);

    if (circleTexture_ and bgTexture_)
    {
        loadingScreenRenderer = std::make_unique<LoadingScreenRenderer>(graphicsApi_, *bgTexture_, *circleTexture_);
        loadingScreenRenderer->init();
    }
    else
    {
        LOG_ERROR << "background or circle texure creatrion error.";
    }
}

bool SceneLoader::IsGpuLoading()
{
    return gpuLoader_.CountOfProcessedTasks() > 0;
}

void SceneLoader::UpdateScreen()
{
    if (loadingScreenRenderer)
    {
        loadingScreenRenderer->render();
    }
    displayManager_.UpdateWindow();
}

void SceneLoader::ScreenRenderLoop()
{
    Utils::Time::CTimeMeasurer timeMeasurer(30, 1000);
    std::unique_ptr<std::thread> processEvents;

    while (isReading or IsGpuLoading())
    {
        timeMeasurer.StartFrame();
        displayManager_.ProcessEvents();
        gpuLoader_.RuntimeGpuTasks();
        UpdateScreen();
        timeMeasurer.EndFrame();
    }
}

void SceneLoader::CheckObjectCount(Scene& scene)
{
    objectCount_ = scene.objectCount;
    if (objectCount_ <= 0)
        objectCount_ = 1;
}
}  // namespace GameEngine
