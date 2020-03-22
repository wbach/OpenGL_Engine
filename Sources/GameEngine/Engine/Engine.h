#pragma once
#include "../Display/DisplayManager.hpp"
#include "../Input/InputManager.h"
#include "../Renderers/RenderersManager.h"
#include "../Scene/SceneManager.h"
#include "EngineEvent.h"
#include "GameEngine/Physics/IPhysicsApi.h"
#include "IntroRenderer.h"
#include "ThreadSync.h"

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
    Engine(std::unique_ptr<GraphicsApi::IGraphicsApi> graphicsApi, std::unique_ptr<Physics::IPhysicsApi> physicsApi,
           SceneFactoryBasePtr sceneFactory);
    ~Engine();
    void Init();
    void GameLoop();
    void AddEngineEvent(EngineEvent event);
    DisplayManager& GetDisplayManager();
    SceneManager& GetSceneManager();

private:
    void SetDisplay();
    void MainLoop();
    void ProcessEngineEvents();
    void Quit();

private:
    std::unique_ptr<GraphicsApi::IGraphicsApi> graphicsApi_;
    std::unique_ptr<Physics::IPhysicsApi> physicsApi_;
    DisplayManager displayManager_;
    std::unique_ptr<Input::InputManager> inputManager_;
    Renderer::Gui::GuiContext guiContext_;
    Renderer::RenderersManager renderersManager_;
    SceneManager sceneManager_;
    IntroRenderer introRenderer_;
    std::mutex engineEventsMutex;
    std::list<EngineEvent> engineEvents;
    bool isRunning_;
};
}  // namespace GameEngine
