#pragma once
#include "../Display/DisplayManager.hpp"
#include "../Input/InputManager.h"
#include "../Renderers/RenderersManager.h"
#include "../Scene/SceneManager.h"
#include "EngineEvent.h"
#include "GameEngine/Api/IGraphicsApi.h"
#include "GameEngine/Physics/IPhysicsApi.h"
#include "IntroRenderer.h"
#include "ThreadSync.h"

namespace GameEngine
{
class DisplayManager;
class LoadingScreenRenderer;

class Engine
{
private:
    std::shared_ptr<DisplayManager> displayManager;

public:
    Engine(IGraphicsApiPtr graphicsApi, Physics::IPhysicsApiPtr physicsApi, SceneFactoryBasePtr sceneFactory);
    ~Engine();
    void Init();
    void GameLoop();
    void AddEngineEvent(EngineEvent event);
    void Render();
    DisplayManager& GetDisplayManager();

public:
    Renderer::Gui::GuiContext guiContext_;
    InputManagerPtr inputManager_;
    Renderer::RenderersManager renderersManager_;
    SceneManager sceneManager_;

private:
    void SetDisplay();
    void MainLoop();
    void ProcessEngineEvents();
    void PrepareFrame();

private:
    IGraphicsApiPtr graphicsApi_;
    IntroRenderer introRenderer_;
    std::mutex engineEventsMutex;
    std::list<EngineEvent> engineEvents;
    std::atomic_bool isRunning;
};
}  // GameEngine
