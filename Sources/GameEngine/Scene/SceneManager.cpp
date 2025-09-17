#include "SceneManager.h"

#include "GameEngine/Engine/EngineContext.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "Logger/Log.h"
#include "Scene.hpp"

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

    StartUpdateThreadIfNeeded();

    SetOnSceneLoadDone([&]() { sceneWrapper_.StartActiveScene(); });
}
SceneManager::~SceneManager()
{
    LOG_DEBUG << "destructor";
    StopThread();
    SetOnSceneLoadDone(nullptr);
    EngineConf.renderer.fpsLimt.unsubscribe(fpsLimitParamSub_);
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
    if (sceneWrapper_.GetState() == SceneWrapperState::ReadyToInitialized)
        sceneWrapper_.Init(onSceneLoadDoneCallback);

    TakeEvents();
    ProccessEvents();
}
void SceneManager::TakeEvents()
{
    auto incomingEvent = GetSceneEvent();

    if (not incomingEvent)
        return;

    StopThread();
    engineContext_.GetRenderersManager().UnSubscribeAll([&, e = *incomingEvent]() { AddEventToProcess(e); });
}
void SceneManager::ProccessEvents()
{
    auto optionalEvent = GetProcessingEvent();

    if (not optionalEvent)
        return;

    auto& e = *optionalEvent;

    switch (e.type)
    {
        case SceneEventType::LOAD_NEXT_SCENE:
            LoadNextScene();
            break;
        case SceneEventType::LOAD_PREVIOUS_SCENE:
            LoadPreviousScene();
            break;
        case SceneEventType::LOAD_SCENE_BY_ID:
            SetSceneToLoad(e.id);
            break;
        case SceneEventType::LOAD_SCENE_BY_NAME:
            SetSceneToLoad(e.name);
            break;
        case SceneEventType::RELOAD_SCENE:
            SetSceneToLoad(currentSceneId_);
            break;
        default:
            break;
    }

    StartUpdateThreadIfNeeded();
}

void SceneManager::SetActiveScene(const std::string& name)
{
    SetSceneToLoad(name);
}

void SceneManager::SetActiveScene(uint32 id)
{
    SetSceneToLoad(id);
}

void SceneManager::Reset()
{
    sceneWrapper_.Reset();
}

void SceneManager::UpdateScene(float dt)
{
    if (not sceneWrapper_.IsInitialized())
        return;

    sceneWrapper_.UpdateScene(dt);
}
void SceneManager::AddSceneEvent(const SceneEvent& e)
{
    std::lock_guard<std::mutex> lk(eventMutex_);
    events_.push(e);
}

void SceneManager::AddEventToProcess(const SceneEvent& e)
{
    std::lock_guard<std::mutex> lk(processingEventMutex_);
    processingEvents_.push(e);
}

std::optional<GameEngine::SceneEvent> SceneManager::GetSceneEvent()
{
    std::lock_guard<std::mutex> lk(eventMutex_);

    if (events_.empty())
        return std::nullopt;

    auto e = events_.front();
    events_.pop();
    return std::move(e);
}

std::optional<GameEngine::SceneEvent> SceneManager::GetProcessingEvent()
{
    std::lock_guard<std::mutex> lk(processingEventMutex_);

    if (processingEvents_.empty())
        return {};

    auto e = processingEvents_.front();
    processingEvents_.pop();

    return std::move(e);
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

template <class SceneNameOrId>
void SceneManager::SetToWrapper(SceneNameOrId sceneNameOrId)
{
    sceneWrapper_.Reset();
    sceneWrapper_.Set(sceneNameOrId, std::bind(&SceneManager::AddSceneEvent, this, std::placeholders::_1));
}

void SceneManager::SetSceneToLoad(uint32 id)
{
    if (!sceneFactory_->IsExist(id))
    {
        LOG_ERROR << "SceneManager::SetSceneToLoad() no more scenes found.";
        return;
    }
    currentSceneId_ = id;
    SetToWrapper<uint32>(id);
}

void SceneManager::SetSceneToLoad(const std::string& name)
{
    if (!sceneFactory_->IsExist(name))
    {
        LOG_ERROR << "SceneManager::SetSceneToLoad() " << name << " not found.";
        return;
    }

    currentSceneId_ = sceneFactory_->GetSceneId(name);
    SetToWrapper<const std::string&>(name);
}
void SceneManager::SetSceneContext(Scene* scene)
{
    scene->SetAddSceneEventCallback(std::bind(&SceneManager::AddSceneEvent, this, std::placeholders::_1));
}

void SceneManager::StartUpdateThreadIfNeeded()
{
    if (not isRunning_)
    {
        LOG_DEBUG << "Starting scene update thread";
        updateSceneThreadId_ = engineContext_.GetThreadSync().Subscribe(
            std::bind(&SceneManager::UpdateScene, this, std::placeholders::_1), "UpdateScene", EngineConf.renderer.fpsLimt);
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

}  // namespace GameEngine
