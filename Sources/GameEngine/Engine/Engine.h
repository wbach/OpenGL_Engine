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

struct ReadConfiguration
{
    ReadConfiguration();
};

class Engine
{
public:
    Engine(std::unique_ptr<Physics::IPhysicsApi>,
           std::unique_ptr<SceneFactoryBase>);
    ~Engine();

    void Init();
    void GameLoop();
    SceneManager& GetSceneManager();

private:
    void CheckThreadsBeforeQuit();
    void MainLoop();
    void ProcessEngineEvents();
    void Quit();

private:
    ReadConfiguration readConfiguration_;
    EngineContext engineContext_;
    SceneManager sceneManager_;
    IntroRenderer introRenderer_;
    IdType loggingLvlParamSub_;
    IdType fpsLimitParamSub_;
    IdType showPhycicsVisualizationSub_;
    IdType physicsThreadId_;
    bool isRunning_;
};
}  // namespace GameEngine
