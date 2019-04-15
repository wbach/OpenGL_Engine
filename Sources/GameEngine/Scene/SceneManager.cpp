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
                           Renderer::RenderersManager& renderersManager, Renderer::Gui::GuiContext& guiContext)
    : grahpicsApi_(grahpicsApi)
    , physicsApi_(physicsApi)
    , sceneFactory_(sceneFactory)
    , currentSceneId_(0)
    , sceneWrapper_(grahpicsApi, displayManager, shaderFactory)
    , displayManager_(displayManager)
    , inputManager_(inputManager)
    , renderersManager_(renderersManager)
    , guiContext_(guiContext)
{
    threadSync_.Subscribe(std::bind(&SceneManager::UpadteScene, this, std::placeholders::_1));
    threadSync_.Start();
    isRunning_ = true;
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
    if (obj != nullptr && !obj->isLoadedToGpu())
        obj->GpuLoadingPass();

    gpuLoader.CallFunctions();
}
void SceneManager::Update()
{
    if (sceneWrapper_.GetState() == SceneWrapperState::ReadyToInitialized)
        sceneWrapper_.Init();

    ProccessEvents();
}
void SceneManager::ProccessEvents()
{
    auto optionalEvent = GetSceneEvent();

    if (!optionalEvent)
        return;

    auto e = optionalEvent.value();

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
        default:
            break;
    }
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

void SceneManager::UpadteScene(float dt)
{
    if (!sceneWrapper_.IsInitialized())
        return;

    sceneWrapper_.Get()->FullUpdate(dt);
}
void SceneManager::AddSceneEvent(const SceneEvent& e)
{
    std::lock_guard<std::mutex> lk(eventMutex_);
    events_.push(e);
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
    renderersManager_.UnSubscribeAll();
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

}  // namespace GameEngine
