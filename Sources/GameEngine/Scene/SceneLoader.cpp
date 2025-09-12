#include "SceneLoader.h"

#include <Logger/Log.h>
#include <Utils/Time/TimeMeasurer.h>
#include <Utils/Time/Timer.h>

#include "GameEngine/Display/DisplayManager.hpp"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/LoadingScreenRenderer.h"
#include "GameEngine/Resources/IGpuResourceLoader.h"
#include "GameEngine/Resources/Textures/GeneralTexture.h"
#include "ISceneFactory.h"
#include "Scene.hpp"

namespace GameEngine
{
SceneLoader::SceneLoader(ISceneFactory& factory, GraphicsApi::IGraphicsApi& graphicsApi, IGpuResourceLoader& gpuResourceLoader,
                         DisplayManager& displayManager)
    : sceneFactory_{factory}
    , graphicsApi_(graphicsApi)
    , displayManager_(displayManager)
    , isLoading_(true)
    , objectCount_(0)
    , loadingScreenRenderer(nullptr)
    , resorceManager_(graphicsApi, gpuResourceLoader)
    , gpuLoader_(gpuResourceLoader)
    , bgTexture_(nullptr)
    , circleTexture_(nullptr)
{
}

SceneLoader::~SceneLoader()
{
    CleanUp();
}

void SceneLoader::CleanUp()
{
    loadingScreenRenderer->cleanUp();
    loadingScreenRenderer.reset();

    if (bgTexture_)
        resorceManager_.GetTextureLoader().DeleteTexture(*bgTexture_);
    if (circleTexture_)
        resorceManager_.GetTextureLoader().DeleteTexture(*circleTexture_);

    bgTexture_     = nullptr;
    circleTexture_ = nullptr;
}

std::unique_ptr<Scene> SceneLoader::Load(uint32 id)
{
    Init();
    IsLoading(true);

    /* LOG TO FIX*/  LOG_ERROR << ("Load scene :" + std::to_string(id));

    std::unique_ptr<Scene> scene;
    std::thread loadingThread([&]() { scene = LoadScene(id); });

    ScreenRenderLoop();
    loadingThread.join();

    return scene;
}

std::unique_ptr<Scene> SceneLoader::Load(const std::string& name)
{
    Init();
    IsLoading(true);

    /* LOG TO FIX*/  LOG_ERROR << ("Load scene :" + name);
    std::unique_ptr<Scene> scene;
    std::thread loadingThread([&]() { scene = LoadScene(name); });
    /* LOG TO FIX*/  LOG_ERROR << ("loadingThread done: " + name);
    ScreenRenderLoop();
    loadingThread.join();

    return scene;
}

template <typename T>
std::unique_ptr<Scene> SceneLoader::LoadScene(T t)
{
    /* LOG TO FIX*/  LOG_ERROR << ("Load scene thread started.");
    Utils::Timer timer;
    auto scene = sceneFactory_.Create(t);
    scene->Init();
    /* LOG TO FIX*/  LOG_ERROR << ("Scene loading time: " + std::to_string(timer.GetTimeMiliSeconds()) + "ms.");
    IsLoading(false);
    return scene;
}

void SceneLoader::Init()
{
    TextureParameters params;
    params.loadType        = TextureLoadType::Immediately;
    params.flipMode        = TextureFlip::VERTICAL;
    params.sizeLimitPolicy = SizeLimitPolicy::NoLimited;
    params.filter          = GraphicsApi::TextureFilter::LINEAR;
    params.mimap           = GraphicsApi::TextureMipmap::LINEAR;

    auto& texureLoader = resorceManager_.GetTextureLoader();
    circleTexture_     = texureLoader.LoadTexture(EngineConf.files.loadingScreenCircleTexture, params);
    bgTexture_         = texureLoader.LoadTexture(EngineConf.files.loadingScreenBackgroundTexture, params);

    if (circleTexture_ and bgTexture_)
    {
        loadingScreenRenderer = std::make_unique<LoadingScreenRenderer>(graphicsApi_, *bgTexture_, *circleTexture_);
        loadingScreenRenderer->init();
    }
    else
    {
        /* LOG TO FIX*/  LOG_ERROR << ("background or circle texure creatrion error.");
    }
}

void SceneLoader::IsLoading(bool is)
{
    isLoading_.store(is);
}

bool SceneLoader::IsLoading()
{
    return isLoading_.load() or gpuLoader_.CountObjectsToAdd() > 0 or gpuLoader_.CountObjectsToRelease() > 0 or
           gpuLoader_.CountObjectsToUpdate() > 0;
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

    while (IsLoading())
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
