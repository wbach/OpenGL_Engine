#include "SceneWrapper.h"
#include "Logger/Log.h"
#include "Scene.hpp"
#include "SceneLoader.h"

namespace GameEngine
{
SceneWrapper::SceneWrapper(GraphicsApi::IGraphicsApi& graphicsApi, DisplayManager& displayManager, IGpuResourceLoader& gpuResourceLoader)
    : graphicsApi_(graphicsApi)
    , displayManager_(displayManager)
    , gpuResourceLoader_(gpuResourceLoader)
    , state_(SceneWrapperState::SceneNotSet)
{
}

SceneWrapper::~SceneWrapper()
{
    DEBUG_LOG("destructor");
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
       ERROR_LOG("SceneWrapper::Init() Wrong state.");
        return;
    }
    SaveSetState(SceneWrapperState::Initializing);
    SceneLoader(graphicsApi_, gpuResourceLoader_, displayManager_).Load(*activeScene);
    SaveSetState(SceneWrapperState::Initilaized);
}

bool SceneWrapper::IsInitialized()
{
    return SaveGetState() == SceneWrapperState::Initilaized;
}

void SceneWrapper::UpdateScene(float dt)
{
    if (not activeScene)
        return;

    activeScene->FullUpdate(dt);
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
       ERROR_LOG("SceneWrapper::Get() scene is nullptr. Probably are not set active scene.");
    }

    return nullptr;
}

SceneWrapperState SceneWrapper::GetState()
{
    if (not activeScene)
        return SceneWrapperState::SceneNotSet;

    return SaveGetState();
}
void SceneWrapper::Reset()
{
    SaveSetState(SceneWrapperState::SceneNotSet);
    activeScene.reset(nullptr);
}
}  // GameEngine
