#include "SceneLoader.h"

#include <Logger/Log.h>
#include <Utils/Time/Timer.h>

#include "GameEngine/Display/DisplayManager.hpp"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/LoadingScreenRenderer.h"
#include "GameEngine/Resources/IGpuResourceLoader.h"
#include "GameEngine/Resources/Textures/GeneralTexture.h"
#include "Scene.hpp"

namespace GameEngine
{
SceneLoader::SceneLoader(GraphicsApi::IGraphicsApi& graphicsApi, IGpuResourceLoader& gpuResourceLoader,
                         DisplayManager& displayManager)
    : graphicsApi_(graphicsApi)
    , displayManager_(displayManager)
    , isLoading_(true)
    , objectCount_(0)
    , objectLoaded_(0)
    , loadingScreenRenderer(nullptr)
    , resorceManager_(graphicsApi, gpuResourceLoader)
    , gpuLoader_(gpuResourceLoader)
    , bgTexture_(nullptr)
    , circleTexture_(nullptr)
{
}

SceneLoader::~SceneLoader()
{
    loadingScreenRenderer.reset();

    if (bgTexture_)
        resorceManager_.GetTextureLoader().DeleteTexture(*bgTexture_);
    if (circleTexture_)
        resorceManager_.GetTextureLoader().DeleteTexture(*circleTexture_);
}

void SceneLoader::Load(Scene& scene)
{
    Init();
    IsLoading(true);

    std::thread loadingThread([this, &scene]() { LoadScene(scene); });

    CheckObjectCount(scene);
    ScreenRenderLoop();
    loadingThread.join();
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
        ERROR_LOG("background or circle texure creatrion error.");
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
    while (IsLoading())
    {
        displayManager_.ProcessEvents();
        gpuLoader_.RuntimeGpuTasks();
        UpdateScreen();
    }
}

void SceneLoader::CheckObjectCount(Scene& scene)
{
    objectCount_ = scene.objectCount;
    if (objectCount_ <= 0)
        objectCount_ = 1;
}

void SceneLoader::LoadScene(Scene& scene)
{
    DEBUG_LOG("Load scene thread started.");
    Utils::Timer timer;
    scene.Init();
    DEBUG_LOG("Scene loading time: " + std::to_string(timer.GetTimeMiliSeconds()) + "ms.");
    IsLoading(false);
}
}  // namespace GameEngine
