#pragma once
#include <Utils/Time/TimerService.h>

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
class GuiElementFactory;
class Scene;
class ISceneManager;
class DialogueManager;

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
    GuiElementFactory& guiElementFactory_;
    Utils::Time::TimerService& timerService_;
    DialogueManager& dialogueManager_;
};
}  // namespace Components
}  // namespace GameEngine
