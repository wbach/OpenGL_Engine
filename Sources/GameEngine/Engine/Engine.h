#pragma once
#include <Input/InputManager.h>

#include "EngineContext.h"
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

struct ExternalComponents
{
    ExternalComponents();
};

class Engine
{
public:
    Engine(std::unique_ptr<Physics::IPhysicsApi>, std::unique_ptr<ISceneFactory>,
           std::unique_ptr<GraphicsApi::IGraphicsApi> = nullptr);
    ~Engine();

    void Init();
    void GameLoop();
    void MainLoop();
    void CheckThreadsBeforeQuit();
    ISceneManager& GetSceneManager();
    EngineContext& GetEngineContext();

private:
    void ProcessEngineEvents();
    void Quit();

private:
    ReadConfiguration readConfiguration_;
    ExternalComponents externalComponents_;
    EngineContext engineContext_;
    IntroRenderer introRenderer_;
    IdType loggingLvlParamSub_;
    IdType fpsLimitParamSub_;
    IdType showPhycicsVisualizationSub_;
    IdType physicsThreadId_;
    IdType quitApiSubId_;
    bool isRunning_;
};
}  // namespace GameEngine
