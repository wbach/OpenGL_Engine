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
namespace GUI
{
class Manager;
class IElementFactory;
}

class FactionManager;
class IResourceManager;
class IGpuResourceLoader;
class Scene;
class ISceneManager;
class DialogueManager;
class ITweenManager;
class IAudioManager;
class GameState;
class NavigationManager;

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
    GUI::Manager& guiManager_;
    GUI::IElementFactory& guiElementFactory_;
    Utils::Time::ITimerService& timerService_;
    DialogueManager& dialogueManager_;
    ITweenManager& tweenManager;
    IAudioManager& audioManager;
    GameState& gamestate;
    NavigationManager& navigationManager;
    FactionManager& factionManager;
};
}  // namespace Components
}  // namespace GameEngine
