#pragma once
#include <Input/InputManager.h>

#include "EngineContext.h"
#include "GameEngine/Scene/SceneManager.h"
#include "IntroRenderer.h"

namespace GraphicsApi
{
class IGraphicsApi;
}  // namespace GraphicsApi

namespace GameEngine
{
class DisplayManager;
class LoadingScreenRenderer;

class Engine
{
public:
    Engine(std::unique_ptr<GraphicsApi::IGraphicsApi>, std::unique_ptr<Physics::IPhysicsApi>,
           std::unique_ptr<SceneFactoryBase>);
    ~Engine();

    void Init();
    void GameLoop();
    // DisplayManager& GetDisplayManager();
    SceneManager& GetSceneManager();

private:
    void RuntimeGpuTasks();
    void RuntimeLoadObjectToGpu();
    void RuntimeReleaseObjectGpu();
    void RuntimeCallFunctionGpu();
    void CheckThreadsBeforeQuit();
    void SetDisplay();
    void MainLoop();
    void ProcessEngineEvents();
    void Quit();

private:
    EngineContext engineContext_;
    SceneManager sceneManager_;
    IntroRenderer introRenderer_;

    bool isRunning_;
};
}  // namespace GameEngine
