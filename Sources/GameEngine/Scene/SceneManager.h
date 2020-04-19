#pragma once
#include <queue>

#include "GameEngine/Engine/EngineEvent.h"
#include "GameEngine/Physics/IPhysicsApi.h"
#include "Mutex.hpp"
#include "SceneEvents.h"
#include "SceneFactoryBase.h"
#include "SceneWrapper.h"
#include "ThreadSync.h"
#include "optional.hpp"

namespace Input
{
class InputManager;
}  // namespace Input

namespace GameEngine
{
namespace Renderer
{
namespace Gui
{
struct GuiContext;
}

class RenderersManager;
}  // namespace Renderer

class SceneManager
{
public:
    SceneManager(GraphicsApi::IGraphicsApi&, Physics::IPhysicsApi&, SceneFactoryBasePtr, DisplayManager&,
                 Input::InputManager&, Renderer::RenderersManager&, Renderer::Gui::GuiContext&,
                 std::function<void(EngineEvent)>);
    ~SceneManager();

    Scene* GetActiveScene();
    void InitActiveScene();
    void RuntimeGpuTasks();
    void Update();
    void SetActiveScene(const std::string& name);
    void Reset();
    void SetFactor();
    void Stop();

private:
    void RuntimeLoadObjectToGpu();
    void RuntimeReleaseObjectGpu();
    void RuntimeCallFunctionGpu();
    void TakeEvents();
    void ProccessEvents();
    void UpdateScene(float dt);
    void AddSceneEvent(const SceneEvent&);
    void AddEventToProcess(const SceneEvent&);
    std::optional<GameEngine::SceneEvent> GetSceneEvent();
    std::optional<GameEngine::SceneEvent> GetProcessingEvent();

    void LoadNextScene();
    void LoadPreviousScene();
    void LoadScene(const std::string&);
    void LoadScene(uint32 id);
    void SetSceneContext(Scene* scene);

    template <class T>
    void JustLoadScene(T scene);

    void Start();

private:
    GraphicsApi::IGraphicsApi& grahpicsApi_;
    Physics::IPhysicsApi& physicsApi_;
    SceneFactoryBasePtr sceneFactory_;

    uint32 currentSceneId_;
    SceneWrapper sceneWrapper_;

    std::mutex eventMutex_;
    std::mutex processingEventMutex_;
    std::queue<GameEngine::SceneEvent> events_;
    std::queue<GameEngine::SceneEvent> processingEvents_;

    DisplayManager& displayManager_;
    Input::InputManager& inputManager_;
    Renderer::RenderersManager& renderersManager_;
    Renderer::Gui::GuiContext& guiContext_;
    std::function<void(EngineEvent)> addEngineEvent_;
    uint32 updateSceneThreadId_;
    bool isRunning_;
};
}  // namespace GameEngine
