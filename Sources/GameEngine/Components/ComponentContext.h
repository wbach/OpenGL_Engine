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
    ComponentContext(Scene& scene, ISceneManager& sceneManager, GraphicsApi::IGraphicsApi& graphicsApi, IGpuResourceLoader& gpuResourceLoader,
                     Time& time, Input::InputManager& input, Physics::IPhysicsApi& physicsApi,
                     IResourceManager& resourceManager, Renderer::RenderersManager& renderersManager,
                     ComponentController& componentController, GuiElementFactory& guiElementFactory, Utils::Time::TimerService& timerService)
        : scene_(scene)
        , sceneManager_(sceneManager)
        , graphicsApi_(graphicsApi)
        , gpuResourceLoader_(gpuResourceLoader)
        , time_(time)
        , inputManager_(input)
        , physicsApi_(physicsApi)
        , resourceManager_(resourceManager)
        , componentController_(componentController)
        , renderersManager_(renderersManager)
        , guiElementFactory_(guiElementFactory)
        , timerService_(timerService)
    {
    }
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
    // ComponentIdsMap ids_;
};
}  // namespace Components
}  // namespace GameEngine
