#include "SceneWrapper.h"

#include <Logger/Log.h>
#include <memory>

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
{
}

SceneWrapper::~SceneWrapper()
{
    LOG_DEBUG << "destructor";
}

void SceneWrapper::Set(ScenePtr scene)
{
    std::lock_guard<std::mutex> lk(sceneMutex);
    activeScene = std::move(scene);
}

void SceneWrapper::UpdateScene(float dt)
{
    std::lock_guard<std::mutex> lk(sceneMutex);
    if (not activeScene)
        return;

    activeScene->FullUpdate(dt);
}

Scene* SceneWrapper::Get()
{
    std::lock_guard<std::mutex> lk(sceneMutex);
    return activeScene.get();
}

void SceneWrapper::StartActiveScene()
{
    std::lock_guard<std::mutex> lk(sceneMutex);
    if (activeScene)
    {
        activeScene->Start();
    }
}
void SceneWrapper::Reset()
{
    std::lock_guard<std::mutex> lk(sceneMutex);
    activeScene.reset(nullptr);
}
}  // namespace GameEngine
