#include "SceneLoader.h"

#include <Logger/Log.h>
#include <Types.h>
#include <Utils.h>
#include <Utils/Time/TimeMeasurer.h>
#include <Utils/Time/Timer.h>

#include <atomic>
#include <memory>
#include <thread>

#include "GameEngine/Display/DisplayManager.hpp"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/LoadingScreenRenderer.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/IResourceManagerFactory.h"
#include "GameEngine/Resources/ITextureLoader.h"
#include "GameEngine/Resources/Textures/GeneralTexture.h"
#include "GameEngine/Scene/Scene.hpp"
#include "ISceneFactory.h"

namespace GameEngine
{
SceneLoader::SceneLoader(ISceneFactory& factory, GraphicsApi::IGraphicsApi& graphicsApi,
                         IResourceManagerFactory& resourceManagerFactory)
    : sceneFactory_{factory}
    , resourceManager(resourceManagerFactory.create())
    , graphicsApi_(graphicsApi)
    , isReading(true)
    , loadingScreenRenderer(nullptr)
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

void SceneLoader::Load(uint32 id)
{
    LoadScene(id);
}

void SceneLoader::Load(const std::string& name)
{
    LoadScene(name);
}

template <typename T>
void SceneLoader::LoadScene(const T& t)
{
    LOG_DEBUG << "Load scene :" << t;
    Init();
    isReading.store(true);
    std::thread(
        [&, requestedScene = t]()
        {
            LOG_DEBUG << "Load scene thread started.";
            Utils::Timer timer;
            resultScene = sceneFactory_.Create(requestedScene);
            LOG_DEBUG << "scene created";
            resultScene->Init();
            resultScene->ProcessEvents();
            isReading.store(false);
            LOG_DEBUG << "Scene read time: " << timer.GetTimeMiliSeconds() << "ms.";
        })
        .detach();
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

void SceneLoader::UpdateLoadingScreen()
{
    if (loadingScreenRenderer)
    {
        loadingScreenRenderer->render();
    }
}

bool SceneLoader::IsReading() const
{
    return isReading;
}

std::unique_ptr<Scene> SceneLoader::GetResultScene()
{
    return std::move(resultScene);
}

}  // namespace GameEngine
