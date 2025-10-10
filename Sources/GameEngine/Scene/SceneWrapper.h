#pragma once
#include <Types.h>

#include <memory>
#include <mutex>

namespace GraphicsApi
{
class IGraphicsApi;
}

namespace GameEngine
{
class Scene;
class ISceneFactory;
class IResourceManagerFactory;
class DisplayManager;
class IGpuResourceLoader;
using ScenePtr = std::unique_ptr<Scene>;

class SceneWrapper
{
public:
    SceneWrapper(ISceneFactory&, GraphicsApi::IGraphicsApi&, DisplayManager&, IGpuResourceLoader&, IResourceManagerFactory&);
    ~SceneWrapper();
    Scene* Get();
    void StartActiveScene();
    void Reset();
    void Set(ScenePtr);
    void UpdateScene(float);

private:
    ISceneFactory& sceneFactory_;
    GraphicsApi::IGraphicsApi& graphicsApi;
    DisplayManager& displayManager;
    IGpuResourceLoader& gpuResourceLoader;
    IResourceManagerFactory& resourceManagerFactory;
    ScenePtr activeScene;
    std::mutex sceneMutex;
};
}  // namespace GameEngine
