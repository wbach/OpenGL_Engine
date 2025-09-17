#include "SceneWrapper.h"

#include <Logger/Log.h>

#include "Scene.hpp"
#include "SceneLoader.h"
#include "magic_enum/magic_enum.hpp"

namespace GameEngine
{
SceneWrapper::SceneWrapper(ISceneFactory& sceneFactory, GraphicsApi::IGraphicsApi& graphicsApi, DisplayManager& displayManager,
                           IGpuResourceLoader& gpuResourceLoader, IResourceManagerFactory& resourceManagerFactory)
    : sceneFactory_{sceneFactory}
    , graphicsApi{graphicsApi}
    , displayManager{displayManager}
    , gpuResourceLoader{gpuResourceLoader}
    , resourceManagerFactory{resourceManagerFactory}
    , state_(SceneWrapperState::SceneNotSet)
{
}

SceneWrapper::~SceneWrapper()
{
    LOG_DEBUG << "destructor";
}

void SceneWrapper::Set(uint32 id, AddEvent sceneEventCallback)
{
    LOG_DEBUG << "Set id: " << id;
    Reset();
    sceneToLoad_          = id;
    addSceneEventCallback = sceneEventCallback;
    SafeSetState(SceneWrapperState::ReadyToInitialized);
}

void SceneWrapper::Set(const std::string& name, AddEvent sceneEventCallback)
{
    LOG_DEBUG << "Set name: " << name;
    Reset();
    sceneToLoad_          = name;
    addSceneEventCallback = sceneEventCallback;
    SafeSetState(SceneWrapperState::ReadyToInitialized);
}

void SceneWrapper::Init(std::function<void()> onLoadDone)
{
    std::lock_guard<std::mutex> lk(initMutex_);

    if (SafeGetState() != SceneWrapperState::ReadyToInitialized)
    {
        LOG_ERROR << "SceneWrapper::Init() Wrong state.";
        return;
    }

    SafeSetState(SceneWrapperState::Initializing);
    std::visit(
        [&](const auto& s)
        {
            SceneLoader sceneLoader(sceneFactory_, graphicsApi, gpuResourceLoader, displayManager, resourceManagerFactory);
            activeScene = sceneLoader.Load(s);
            activeScene->SetAddSceneEventCallback(addSceneEventCallback);
            if (onLoadDone)
            {
                onLoadDone();
            }
        },
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
    LOG_DEBUG << "SetState = " << magic_enum::enum_name(state);
    std::lock_guard<std::mutex> lk(stateMutex_);
    state_ = state;
}

Scene* SceneWrapper::Get()
{
    if (SafeGetState() == SceneWrapperState::SceneNotSet)
    {
        LOG_ERROR << "SceneWrapper::Get() scene is nullptr. Probably are not set active scene.";
    }

    return activeScene.get();
}

SceneWrapperState SceneWrapper::GetState()
{
    return SafeGetState();
}

void SceneWrapper::StartActiveScene()
{
    if (activeScene)
    {
        activeScene->Start();
    }
}
void SceneWrapper::Reset()
{
    SafeSetState(SceneWrapperState::SceneNotSet);
    activeScene.reset(nullptr);
}
}  // namespace GameEngine
