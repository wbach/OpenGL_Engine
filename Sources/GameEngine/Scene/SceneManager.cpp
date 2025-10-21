#include "SceneManager.h"

#include <Utils/IThreadSync.h>
#include <Utils/ThreadSubscriber.h>

#include "GameEngine/Engine/EngineContext.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "Logger/Log.h"
#include "Scene.hpp"
#include "SceneLoader.h"

namespace GameEngine
{
std::unique_ptr<ISceneFactory> setEngineContext(EngineContext& context, std::unique_ptr<ISceneFactory> factory)
{
    factory->SetEngineContext(context);
    return factory;
}

SceneManager::SceneManager(EngineContext& engineContext, std::unique_ptr<ISceneFactory> sceneFactory)
    : engineContext_(engineContext)
    , sceneFactory_(setEngineContext(engineContext, std::move(sceneFactory)))
    , sceneWrapper_(*sceneFactory_, engineContext.GetGraphicsApi(), engineContext.GetDisplayManager(),
                    engineContext.GetGpuResourceLoader(), engineContext.GetResourceManagerFactory())
    , currentSceneId_(0)
    , isRunning_(false)
{
    fpsLimitParamSub_ = EngineConf.renderer.fpsLimt.subscribeForChange(
        [this]()
        {
            if (updateSceneThreadId_)
            {
                engineContext_.GetThreadSync().GetSubscriber(*updateSceneThreadId_)->SetFpsLimit(EngineConf.renderer.fpsLimt);
            }
        });

    SetOnSceneLoadDone([&]() { sceneWrapper_.StartActiveScene(); });
}
SceneManager::~SceneManager()
{
    LOG_DEBUG << "destructor";
    StopThread();
    SetOnSceneLoadDone(nullptr);
    EngineConf.renderer.fpsLimt.unsubscribe(fpsLimitParamSub_);
    sceneWrapper_.Reset();
}
Scene* SceneManager::GetActiveScene()
{
    return sceneWrapper_.Get();
}
void SceneManager::SetOnSceneLoadDone(OnSceneLoadDoneCallback callback)
{
    LOG_DEBUG << "Set new onSceneLoadDoneCallback";
    onSceneLoadDoneCallback = callback;
}
void SceneManager::Update()
{
    if (sceneLoader != nullptr)
    {
        sceneLoader->UpdateLoadingScreen();
        engineContext_.GetDisplayManager().UpdateWindow();

        if (sceneLoader->IsReading() or IsGpuLoading())
        {
            return;
        }

        sceneWrapper_.Set(sceneLoader->GetResultScene());
        sceneLoader.reset();
        LOG_DEBUG << "Scene load done. Callback";
        if (onSceneLoadDoneCallback)
        {
            onSceneLoadDoneCallback();
        }

        StartUpdateThreadIfNeeded();
    }
}
void SceneManager::ProcessEvent(const ChangeSceneEvent& e)
{
    LOG_DEBUG << "ProcessEvent ChangeSceneEvent : " << magic_enum::enum_name(e.type);
    StopThread();
    engineContext_.GetRenderersManager().UnSubscribeAll(
        [&, changeSceneEvent = e]() { engineContext_.AddEngineEvent(ChangeSceneConfirmEvent{.event = changeSceneEvent}); });
}
void SceneManager::ProcessEvent(const ChangeSceneConfirmEvent& e)
{
    LOG_DEBUG << "ProcessEvent ChangeSceneConfirmEvent : " << magic_enum::enum_name(e.event.type);
    switch (e.event.type)
    {
        case ChangeSceneEvent::Type::LOAD_NEXT_SCENE:
            LoadNextScene();
            break;
        case ChangeSceneEvent::Type::LOAD_PREVIOUS_SCENE:
            LoadPreviousScene();
            break;
        case ChangeSceneEvent::Type::LOAD_SCENE_BY_ID:
            SetSceneToLoad(e.event.id);
            break;
        case ChangeSceneEvent::Type::LOAD_SCENE_BY_NAME:
            SetSceneToLoad(e.event.name);
            break;
        case ChangeSceneEvent::Type::RELOAD_SCENE:
            SetSceneToLoad(currentSceneId_);
            break;
        default:
            break;
    }
}

void SceneManager::SetActiveScene(const std::string& name)
{
    SetSceneToLoad(name);
    currentSceneId_ = sceneFactory_->GetSceneId(name);
}

void SceneManager::SetActiveScene(uint32 id)
{
    SetSceneToLoad(id);
    currentSceneId_ = id;
}

void SceneManager::Reset()
{
    sceneWrapper_.Reset();
}

void SceneManager::UpdateScene(float dt)
{
    sceneWrapper_.UpdateScene(dt);
}

void SceneManager::LoadNextScene()
{
    if (currentSceneId_ >= sceneFactory_->ScenesSize() - 1)
    {
        LOG_WARN << "SceneManager::LoadNextScene() no more scenes found.";
        return;
    }

    SetSceneToLoad(++currentSceneId_);
}
void SceneManager::LoadPreviousScene()
{
    if (currentSceneId_ == 0)
    {
        LOG_WARN << "SceneManager::LoadPreviousScene() no more scenes found.";
        return;
    }
    SetSceneToLoad(--currentSceneId_);
}

template <typename T>
void SceneManager::SetSceneToLoad(const T& t)
{
    if (not sceneFactory_->IsExist(t))
    {
        LOG_ERROR << "SceneManager::SetSceneToLoad() " << t << " not found.";
        return;
    }
    StopThread();
    sceneWrapper_.Reset();
    sceneLoader = std::make_unique<SceneLoader>(*sceneFactory_, engineContext_.GetGraphicsApi(),
                                                engineContext_.GetResourceManagerFactory());
    sceneLoader->Load(t);
}

void SceneManager::StartUpdateThreadIfNeeded()
{
    if (not isRunning_)
    {
        LOG_DEBUG << "Starting scene update thread";
        updateSceneThreadId_ = engineContext_.GetThreadSync().Subscribe(
            [this](float deltaTime) { sceneWrapper_.UpdateScene(deltaTime); }, "UpdateScene", EngineConf.renderer.fpsLimt);
        isRunning_ = true;
    }
    else
    {
        LOG_WARN << "Scene is already started!";
    }
}

void SceneManager::StopThread()
{
    if (isRunning_)
    {
        LOG_DEBUG << "Stopping scene thread";
        if (updateSceneThreadId_)
            engineContext_.GetThreadSync().Unsubscribe(*updateSceneThreadId_);
        isRunning_ = false;
    }
    else
    {
        LOG_WARN << "Scene thread is not started.";
    }
}

const IdMap& SceneManager::GetAvaiableScenes() const
{
    return sceneFactory_->GetAvaiableScenes();
}

bool SceneManager::IsGpuLoading() const
{
    return engineContext_.GetGpuResourceLoader().CountOfProcessedTasks() > 0;
}

}  // namespace GameEngine
