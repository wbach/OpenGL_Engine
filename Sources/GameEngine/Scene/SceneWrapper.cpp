#include "SceneWrapper.h"
#include "Logger/Log.h"
#include "Scene.hpp"
#include "SceneLoader.h"

namespace GameEngine
{
SceneWrapper::SceneWrapper(IGraphicsApi& graphicsApi, std::shared_ptr<DisplayManager>& displayManager,
                           IShaderFactory& shaderFactory)
    : graphicsApi_(graphicsApi)
    , shaderFactory_(shaderFactory)
    , displayManager_(displayManager)
    , state_(SceneWrapperState::SceneNotSet)
{
}

void SceneWrapper::Set(ScenePtr scene)
{
    Reset();
    activeScene = std::move(scene);
    SaveSetState(SceneWrapperState::ReadyToInitialized);
}

void SceneWrapper::Init()
{
    std::lock_guard<std::mutex> lk(initMutex_);

    if (SaveGetState() != SceneWrapperState::ReadyToInitialized)
    {
        Error("SceneWrapper::Init() Wrong state.");
        return;
    }
    SaveSetState(SceneWrapperState::Initializing);

    SceneLoader sceneloader(graphicsApi_, displayManager_, shaderFactory_);
    if (!sceneloader.Load(activeScene.get()))
        return;

    SaveSetState(SceneWrapperState::Initilaized);
}

bool SceneWrapper::IsInitialized()
{
    return SaveGetState() == SceneWrapperState::Initilaized;
}

SceneWrapperState SceneWrapper::SaveGetState()
{
    std::lock_guard<std::mutex> lk(stateMutex_);
    return state_;
}

void SceneWrapper::SaveSetState(SceneWrapperState state)
{
    std::lock_guard<std::mutex> lk(stateMutex_);
    state_ = state;
}

Scene* SceneWrapper::Get()
{
    if (SaveGetState() == SceneWrapperState::Initilaized)
        return activeScene.get();

    if (SaveGetState() == SceneWrapperState::SceneNotSet)
    {
        Error("SceneWrapper::Get() scene is nullptr. Probably are not set active scene.");
    }

    return nullptr;
}

SceneWrapperState SceneWrapper::GetState()
{
    if (activeScene == nullptr)
        return SceneWrapperState::SceneNotSet;

    return SaveGetState();
}
void SceneWrapper::Reset()
{
    SaveSetState(SceneWrapperState::SceneNotSet);
    activeScene.reset(nullptr);
}
}  // GameEngine
