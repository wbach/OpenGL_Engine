#pragma once
#include <memory>
#include <vector>
#include "GameEngine/Api/IGraphicsApi.h"
#include "Mutex.hpp"

namespace GameEngine
{
class Scene;
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
    SceneWrapper(IGraphicsApiPtr graphicsApi, std::shared_ptr<DisplayManager>&);

    Scene* Get();
    SceneWrapperState GetState();
    void Reset();
    void Set(ScenePtr scene);
    void Init();
    bool IsInitialized();

private:
    SceneWrapperState SaveGetState();
    void SaveSetState(SceneWrapperState state);

private:
    IGraphicsApiPtr graphicsApi_;
    std::mutex initMutex_;
    std::mutex stateMutex_;

    ScenePtr activeScene;
    std::shared_ptr<DisplayManager>& displayManager_;
    SceneWrapperState state_;
};
}  // GameEngine
