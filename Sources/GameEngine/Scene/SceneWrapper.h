#pragma once
#include <memory>
#include <vector>

#include "GraphicsApi/IGraphicsApi.h"
#include "Mutex.hpp"
#include "SceneEvents.h"

namespace GameEngine
{
class Scene;
class ISceneFactory;
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
    SceneWrapper(ISceneFactory&, GraphicsApi::IGraphicsApi&, DisplayManager&, IGpuResourceLoader&);
    ~SceneWrapper();
    Scene* Get();
    SceneWrapperState GetState();
    void Reset();
    void Set(uint32, AddEvent);
    void Set(const std::string&, AddEvent);
    void Init();
    bool IsInitialized();
    void UpdateScene(float dt);

private:
    SceneWrapperState SafeGetState();
    void SafeSetState(SceneWrapperState state);

private:
    ISceneFactory& sceneFactory_;
    GraphicsApi::IGraphicsApi& graphicsApi_;
    DisplayManager& displayManager_;
    IGpuResourceLoader& gpuResourceLoader_;
    std::mutex initMutex_;
    std::mutex stateMutex_;

    ScenePtr activeScene;
    SceneWrapperState state_;

    std::variant<uint32, std::string> sceneToLoad_;
    AddEvent addSceneEventCallback;
};
}  // namespace GameEngine
