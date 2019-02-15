#pragma once
#include <queue>
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
    SceneManager(GraphicsApi::IGraphicsApi& grahpicsApi, Physics::IPhysicsApi& physicsApi, SceneFactoryBasePtr,
                 std::shared_ptr<DisplayManager>&, IShaderFactory& shaderFactory, std::shared_ptr<Input::InputManager>&,
                 Renderer::RenderersManager&, Renderer::Gui::GuiContext& guiContext);
    ~SceneManager();
    Scene* GetActiveScene();
    void InitActiveScene();
    void RuntimeLoadObjectToGpu();
    void Update();
    void SetActiveScene(const std::string& name);
    void Reset();
    void SetFactor();
    void Stop();
    bool IsRunning() const;

private:
    void ProccessEvents();
    void UpadteScene(float dt);
    void AddSceneEvent(const SceneEvent&);
    wb::optional<GameEngine::SceneEvent> GetSceneEvent();

    void LoadNextScene();
    void LoadPreviousScene();
    void LoadScene(const std::string&);
    void LoadScene(uint32 id);
    void SetSceneContext(Scene* scene);

    template <class T>
    void JustLoadScene(T scene);

private:
    GraphicsApi::IGraphicsApi& grahpicsApi_;
    Physics::IPhysicsApi& physicsApi_;
    SceneFactoryBasePtr sceneFactory_;

    uint32 currentSceneId_;
    SceneWrapper sceneWrapper_;

    std::mutex eventMutex_;
    std::queue<GameEngine::SceneEvent> events_;

    std::shared_ptr<DisplayManager>& displayManager_;
    std::shared_ptr<Input::InputManager>& inputManager_;
    Renderer::RenderersManager& renderersManager_;
    Renderer::Gui::GuiContext& guiContext_;

    Utils::Thread::ThreadSync threadSync_;
    bool isRunning_;
};
}  // namespace GameEngine
