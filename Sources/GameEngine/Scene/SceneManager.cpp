#include "SceneManager.h"
#include "GameEngine/Renderers/GUI/GuiContext.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/GpuResourceLoader.h"
#include "Logger/Log.h"
#include "Scene.hpp"

namespace GameEngine
{
SceneManager::SceneManager(GraphicsApi::IGraphicsApi& grahpicsApi, Physics::IPhysicsApi& physicsApi,
                           SceneFactoryBasePtr sceneFactory, std::shared_ptr<DisplayManager>& displayManager,
                           IShaderFactory& shaderFactory, std::shared_ptr<Input::InputManager>& inputManager,
                           Renderer::RenderersManager& renderersManager, Renderer::Gui::GuiContext& guiContext,
                           std::function<void(EngineEvent)> addEngineEvent)
    : grahpicsApi_(grahpicsApi)
    , physicsApi_(physicsApi)
    , sceneFactory_(sceneFactory)
    , currentSceneId_(0)
    , sceneWrapper_(grahpicsApi, displayManager, shaderFactory)
    , displayManager_(displayManager)
    , inputManager_(inputManager)
    , renderersManager_(renderersManager)
    , guiContext_(guiContext)
    , addEngineEvent_(addEngineEvent)
{
    UpdateSubscribe();
}
SceneManager::~SceneManager()
{
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
void SceneManager::RuntimeLoadObjectToGpu()
{
    if (!sceneWrapper_.IsInitialized())
        return;

    auto& gpuLoader = sceneWrapper_.Get()->GetResourceManager().GetGpuResourceLoader();

    auto obj = gpuLoader.GetObjectToGpuLoadingPass();

    while (obj != nullptr)
    {
        if (obj->IsLoadedToGpu())
            obj->GpuLoadingPass();

       obj = gpuLoader.GetObjectToGpuLoadingPass();
    }
}
void SceneManager::RuntimeReleaseObjectGpu()
{
    auto& gpuLoader = sceneWrapper_.Get()->GetResourceManager().GetGpuResourceLoader();

    auto obj = gpuLoader.GetObjectToRelease();

    while (obj)
    {
        grahpicsApi_.DeleteObject(*obj);
        obj = gpuLoader.GetObjectToRelease();
    }
}
void SceneManager::RuntimeCallFunctionGpu()
{
    sceneWrapper_.Get()->GetResourceManager().GetGpuResourceLoader().CallFunctions();
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
    auto optionalEvent = GetSceneEvent();

    if (!optionalEvent)
        return;

    auto& e = optionalEvent.value();

    StopUpdateSubscribe();
    renderersManager_.UnSubscribeAll([&, e]() { AddEventToProcess(e); });
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

    UpdateSubscribe();
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
    sceneFactory_->SetGraphicsApi(grahpicsApi_);
    sceneFactory_->SetDisplayManager(displayManager_.get());
    sceneFactory_->SetInputManager(inputManager_.get());
    sceneFactory_->SetRenderersManager(&renderersManager_);
    sceneFactory_->SetPhysicsApi(physicsApi_);
}

void SceneManager::Stop()
{
    if (not isRunning_)
        return;

    threadSync_.Stop();
    isRunning_ = false;
}

bool SceneManager::IsRunning() const
{
    return isRunning_;
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

wb::optional<GameEngine::SceneEvent> SceneManager::GetSceneEvent()
{
    std::lock_guard<std::mutex> lk(eventMutex_);

    if (events_.empty())
        return wb::optional<GameEngine::SceneEvent>();

    auto e = events_.front();
    events_.pop();

    return e;
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
    scene->SetAddEngineEventCallback(addEngineEvent_);
}

void SceneManager::UpdateSubscribe()
{
    DEBUG_LOG("");
    updateSceneThreadId_ =
        threadSync_.Subscribe(std::bind(&SceneManager::UpdateScene, this, std::placeholders::_1), "UpdateScene");
    threadSync_.Start();
    isRunning_ = true;
}

void SceneManager::StopUpdateSubscribe()
{
    DEBUG_LOG("");
    threadSync_.Unsubscribe(updateSceneThreadId_);
    threadSync_.Stop();
    isRunning_ = false;
}

}  // namespace GameEngine
