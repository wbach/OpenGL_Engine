#pragma once
#include "../Display/DisplayManager.hpp"
#include "../Input/InputManager.h"
#include "../Renderers/RenderersManager.h"
#include "../Scene/SceneManager.h"
#include "EngineEvent.h"
#include "GameEngine/Physics/IPhysicsApi.h"
#include "GameEngine/Shaders/ShaderFactory.h"
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
    void Render();
    DisplayManager& GetDisplayManager();
    SceneManager& GetSceneManager();

private:
    void SetDisplay();
    void MainLoop();
    void ProcessEngineEvents();
    void PrepareFrame();

private:
    std::unique_ptr<GraphicsApi::IGraphicsApi> graphicsApi_;
    std::unique_ptr<Physics::IPhysicsApi> physicsApi_;
    std::shared_ptr<DisplayManager> displayManager;
    Renderer::Gui::GuiContext guiContext_;
    Input::InputManagerPtr inputManager_;
    Renderer::RenderersManager renderersManager_;
    SceneManager sceneManager_;
    ShaderFactory shaderFactory_;
    IntroRenderer introRenderer_;
    std::mutex engineEventsMutex;
    std::list<EngineEvent> engineEvents;
    std::atomic_bool isRunning;
};
}  // namespace GameEngine
