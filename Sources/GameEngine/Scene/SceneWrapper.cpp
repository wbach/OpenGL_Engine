#include "SceneWrapper.h"

#include <Logger/Log.h>
#include <Utils/Variant.h>

#include "Scene.hpp"
#include "SceneLoader.h"

namespace GameEngine
{
SceneWrapper::SceneWrapper(ISceneFactory& sceneFactory, GraphicsApi::IGraphicsApi& graphicsApi, DisplayManager& displayManager,
                           IGpuResourceLoader& gpuResourceLoader)
    : sceneFactory_{sceneFactory}
    , graphicsApi_(graphicsApi)
    , displayManager_(displayManager)
    , gpuResourceLoader_(gpuResourceLoader)
    , state_(SceneWrapperState::SceneNotSet)
{
}

SceneWrapper::~SceneWrapper()
{
    DEBUG_LOG("destructor");
}

void SceneWrapper::Set(uint32 id, AddEvent sceneEventCallback)
{
    DEBUG_LOG("Set id");
    Reset();
    sceneToLoad_          = id;
    addSceneEventCallback = sceneEventCallback;
    SafeSetState(SceneWrapperState::ReadyToInitialized);
}

void SceneWrapper::Set(const std::string& name, AddEvent sceneEventCallback)
{
    DEBUG_LOG("Set name");
    Reset();
    sceneToLoad_          = name;
    addSceneEventCallback = sceneEventCallback;
    SafeSetState(SceneWrapperState::ReadyToInitialized);
}

void SceneWrapper::Init()
{
    std::lock_guard<std::mutex> lk(initMutex_);

    if (SafeGetState() != SceneWrapperState::ReadyToInitialized)
    {
        ERROR_LOG("SceneWrapper::Init() Wrong state.");
        return;
    }

    SafeSetState(SceneWrapperState::Initializing);
    std::visit(visitor{[&](const auto& s)
                       {
                           activeScene = SceneLoader(sceneFactory_, graphicsApi_, gpuResourceLoader_, displayManager_).Load(s);
                           activeScene->SetAddSceneEventCallback(addSceneEventCallback);
                       }},
               sceneToLoad_);
    SafeSetState(SceneWrapperState::Initilaized);
}

bool SceneWrapper::IsInitialized()
{
    return SafeGetState() == SceneWrapperState::Initilaized;
}

void SceneWrapper::UpdateScene(float dt)
{
    if (not activeScene)
        return;

    activeScene->FullUpdate(dt);
}

SceneWrapperState SceneWrapper::SafeGetState()
{
    std::lock_guard<std::mutex> lk(stateMutex_);
    return state_;
}

void SceneWrapper::SafeSetState(SceneWrapperState state)
{
    DEBUG_LOG("SetState = " + std::to_string(static_cast<int>(state)));
    std::lock_guard<std::mutex> lk(stateMutex_);
    state_ = state;
}

Scene* SceneWrapper::Get()
{
    if (SafeGetState() == SceneWrapperState::Initilaized)
        return activeScene.get();

    if (SafeGetState() == SceneWrapperState::SceneNotSet)
    {
        ERROR_LOG("SceneWrapper::Get() scene is nullptr. Probably are not set active scene.");
    }

    return nullptr;
}

SceneWrapperState SceneWrapper::GetState()
{
    return SafeGetState();
}
void SceneWrapper::Reset()
{
    SafeSetState(SceneWrapperState::SceneNotSet);
    activeScene.reset(nullptr);
}
}  // namespace GameEngine
