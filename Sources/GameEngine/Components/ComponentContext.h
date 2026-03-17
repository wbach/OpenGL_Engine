#pragma once
#include <Utils/Time/ITimerService.h>

#include "ComponentController.h"
#include "GameEngine/Time/Time.h"

namespace Input
{
class InputManager;
}  // namespace Input
namespace GraphicsApi
{
class IGraphicsApi;
}  // namespace GraphicsApi
namespace GameEngine
{
class IResourceManager;
class IGpuResourceLoader;
class IGuiElementFactory;
class Scene;
class ISceneManager;
class DialogueManager;
class ITweenManager;
class IAudioManager;
class GuiManager;
struct GameState;

namespace Physics
{
class IPhysicsApi;
}  // namespace Physics
namespace Renderer
{
class RenderersManager;
}  // namespace Renderer

namespace Components
{
typedef std::unordered_map<uint32, FunctionType> ComponentIdsMap;

struct ComponentContext
{
    Scene& scene_;
    ISceneManager& sceneManager_;
    GraphicsApi::IGraphicsApi& graphicsApi_;
    IGpuResourceLoader& gpuResourceLoader_;
    Time& time_;
    Input::InputManager& inputManager_;
    Physics::IPhysicsApi& physicsApi_;
    IResourceManager& resourceManager_;
    ComponentController& componentController_;
    Renderer::RenderersManager& renderersManager_;
    GuiManager& guiManager_;
    IGuiElementFactory& guiElementFactory_;
    Utils::Time::ITimerService& timerService_;
    DialogueManager& dialogueManager_;
    ITweenManager& tweenManager;
    IAudioManager& audioManager;
    GameState& gamestate;
};
}  // namespace Components
}  // namespace GameEngine
