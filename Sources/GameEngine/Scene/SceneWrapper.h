#pragma once
#include <memory>
#include <vector>
#include "GraphicsApi/IGraphicsApi.h"
#include "Mutex.hpp"

namespace GameEngine
{
class Scene;
class IGpuResourceLoader;
class DisplayManager;
typedef std::unique_ptr<Scene> ScenePtr;

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
    SceneWrapper(GraphicsApi::IGraphicsApi&, DisplayManager&, IGpuResourceLoader&);
    ~SceneWrapper();
    Scene* Get();
    SceneWrapperState GetState();
    void Reset();
    void Set(ScenePtr scene);
    void Init();
    bool IsInitialized();
    void UpdateScene(float dt);

private:
    SceneWrapperState SaveGetState();
    void SaveSetState(SceneWrapperState state);

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    DisplayManager& displayManager_;
    IGpuResourceLoader& gpuResourceLoader_;
    std::mutex initMutex_;
    std::mutex stateMutex_;

    ScenePtr activeScene;
    SceneWrapperState state_;
};
}  // GameEngine
