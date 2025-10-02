#pragma once
#include "EngineContext.h"
#include "IntroRenderer.h"
#include "ConfigurationReader.h"
#include "ExternalComponentsReader.h"

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
    Engine(std::unique_ptr<Physics::IPhysicsApi>, std::unique_ptr<ISceneFactory>,
           std::unique_ptr<GraphicsApi::IGraphicsApi> = nullptr);
    ~Engine();

    void Init();
    void GameLoop();
    void MainLoop();
    void CheckThreadsBeforeQuit();
    ExternalComponentsReader& getExternalComponentsReader();
    ISceneManager& GetSceneManager();
    EngineContext& GetEngineContext();

private:
    void ProcessEngineEvents();
    void Quit();
    void Quit(QuitEvent);

private:
    ConfigurationReader readConfiguration_;
    EngineContext engineContext_;
    ExternalComponentsReader externalComponents_;
    IntroRenderer introRenderer_;
    IdType loggingLvlParamSub_;
    IdType fpsLimitParamSub_;
    IdType showPhycicsVisualizationSub_;
    IdType physicsThreadId_;
    IdType quitApiSubId_;
    bool isRunning_;
};
}  // namespace GameEngine
