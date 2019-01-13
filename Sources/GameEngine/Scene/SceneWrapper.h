#pragma once
#include <memory>
#include <vector>
#include "GameEngine/Api/IGraphicsApi.h"
#include "Mutex.hpp"

namespace GameEngine
{
class Scene;
class DisplayManager;
class IShaderFactory;
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
    SceneWrapper(IGraphicsApi& graphicsApi, std::shared_ptr<DisplayManager>&, IShaderFactory& shaderFactory);

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
    IGraphicsApi& graphicsApi_;
    IShaderFactory& shaderFactory_;
    std::mutex initMutex_;
    std::mutex stateMutex_;

    ScenePtr activeScene;
    std::shared_ptr<DisplayManager>& displayManager_;
    SceneWrapperState state_;
};
}  // GameEngine
