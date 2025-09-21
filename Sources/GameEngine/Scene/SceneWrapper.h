#pragma once
#include <memory>
#include <mutex>
#include <variant>

#include "SceneEvents.h"

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

enum SceneWrapperState
{
    SceneNotSet,
    ReadyToInitialized,
    Initializing,
    Initilaized
};

class SceneWrapper
{
public:
    SceneWrapper(ISceneFactory&, GraphicsApi::IGraphicsApi&, DisplayManager&, IGpuResourceLoader&, IResourceManagerFactory&);
    ~SceneWrapper();
    Scene* Get();
    SceneWrapperState GetState();
    void StartActiveScene();
    void Reset();
    void Set(uint32);
    void Set(const std::string&);
    void Init(std::function<void()>);
    bool IsInitialized();
    void UpdateScene(float dt);

private:
    SceneWrapperState SafeGetState();
    void SafeSetState(SceneWrapperState state);

private:
    ISceneFactory& sceneFactory_;
    GraphicsApi::IGraphicsApi& graphicsApi;
    DisplayManager& displayManager;
    IGpuResourceLoader& gpuResourceLoader;
    IResourceManagerFactory& resourceManagerFactory;

    std::mutex initMutex_;
    std::mutex stateMutex_;

    ScenePtr activeScene;
    SceneWrapperState state_;

    std::variant<uint32, std::string> sceneToLoad_;
};
}  // namespace GameEngine
