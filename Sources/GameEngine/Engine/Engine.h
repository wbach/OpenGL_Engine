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

struct EnableLogger
{
    EnableLogger();
};

class Engine
{
public:
    Engine(std::unique_ptr<GraphicsApi::IGraphicsApi>, std::unique_ptr<Physics::IPhysicsApi>,
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
    EnableLogger enableLogger_;
    EngineContext engineContext_;
    SceneManager sceneManager_;
    IntroRenderer introRenderer_;
    IdType loggingLvlParamSub_;
    bool isRunning_;
};
}  // namespace GameEngine
