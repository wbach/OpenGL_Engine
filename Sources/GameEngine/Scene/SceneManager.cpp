#include "SceneManager.h"

#include "GameEngine/Engine/EngineContext.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/GpuResourceLoader.h"
#include "Logger/Log.h"
#include "Scene.hpp"

namespace GameEngine
{
SceneManager::SceneManager(EngineContext& engineContext, std::unique_ptr<SceneFactoryBase> sceneFactory)
    : engineContext_(engineContext)
    , sceneFactory_(std::move(sceneFactory))
    , sceneWrapper_(engineContext.GetGraphicsApi(), engineContext.GetDisplayManager(),
                    engineContext.GetGpuResourceLoader())
    , currentSceneId_(0)
    , isRunning_(false)
{
    fpsLimitParamSub_ = EngineConf.renderer.fpsLimt.subscribeForChange(
        [this]()
        {
            if (updateSceneThreadId_)
            {
                engineContext_.GetThreadSync()
                    .GetSubscriber(*updateSceneThreadId_)
                    ->SetFpsLimit(EngineConf.renderer.fpsLimt);
            }
        });

    Start();
}
SceneManager::~SceneManager()
{
    EngineConf.renderer.fpsLimt.unsubscribe(fpsLimitParamSub_);
    DEBUG_LOG("destructor");
    Stop();
}
Scene* SceneManager::GetActiveScene()
{
    return sceneWrapper_.Get();
}
void SceneManager::InitActiveScene()
{
    if (!sceneWrapper_.IsInitialized())
    {
        DEBUG_LOG("SceneManager::InitActiveScene() active scene isn't initialized.");
        return;
    }
}

void SceneManager::Update()
{
    if (sceneWrapper_.GetState() == SceneWrapperState::ReadyToInitialized)
        sceneWrapper_.Init();

    TakeEvents();
    ProccessEvents();
}
void SceneManager::TakeEvents()
{
    auto incomingEvent = GetSceneEvent();

    if (not incomingEvent)
        return;

    Stop();
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
            LoadScene(e.id);
            break;
        case SceneEventType::LOAD_SCENE_BY_NAME:
            LoadScene(e.name);
            break;
        case SceneEventType::RELOAD_SCENE:
            LoadScene(currentSceneId_);
            break;
        default:
            break;
    }

    Start();
}

void SceneManager::SetActiveScene(const std::string& name)
{
    LoadScene(name);
}

void SceneManager::Reset()
{
    sceneWrapper_.Reset();
}

void SceneManager::SetFactor()
{
    sceneFactory_->SetEngineContext(engineContext_);
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
        DEBUG_LOG("SceneManager::LoadNextScene() no more scenes found.");
        return;
    }
    LoadScene(++currentSceneId_);
}
void SceneManager::LoadPreviousScene()
{
    if (currentSceneId_ == 0)
    {
        DEBUG_LOG("SceneManager::LoadPreviousScene() no more scenes found.");
        return;
    }
    LoadScene(--currentSceneId_);
}

template <class T>
void SceneManager::JustLoadScene(T scene)
{
    sceneWrapper_.Reset();
    auto s = sceneFactory_->Create(scene);
    SetSceneContext(s.get());
    sceneWrapper_.Set(std::move(s));
}

void SceneManager::LoadScene(uint32 id)
{
    if (!sceneFactory_->IsExist(id))
    {
        DEBUG_LOG("SceneManager::LoadScene() no more scenes found.");
        return;
    }
    currentSceneId_ = id;
    JustLoadScene<uint32>(id);
}

void SceneManager::LoadScene(const std::string& name)
{
    if (!sceneFactory_->IsExist(name))
    {
        DEBUG_LOG("SceneManager::LoadScene() " + name + " not found.");
        return;
    }

    currentSceneId_ = sceneFactory_->GetSceneId(name);
    JustLoadScene<const std::string&>(name);
}
void SceneManager::SetSceneContext(Scene* scene)
{
    scene->SetAddSceneEventCallback(std::bind(&SceneManager::AddSceneEvent, this, std::placeholders::_1));
}

void SceneManager::Start()
{
    if (not isRunning_)
    {
        DEBUG_LOG("Starting scene");
        updateSceneThreadId_ =
            engineContext_.GetThreadSync().Subscribe(std::bind(&SceneManager::UpdateScene, this, std::placeholders::_1),
                                                     "UpdateScene", EngineConf.renderer.fpsLimt);
        isRunning_ = true;
    }
    else
    {
        ERROR_LOG("Scene is already started!");
    }
}

void SceneManager::Stop()
{
    if (isRunning_)
    {
        DEBUG_LOG("Stopping scene");
        if (updateSceneThreadId_)
            engineContext_.GetThreadSync().Unsubscribe(*updateSceneThreadId_);
        isRunning_ = false;
    }
    else
    {
        WARNING_LOG("Scene is not started.");
    }
}

}  // namespace GameEngine
