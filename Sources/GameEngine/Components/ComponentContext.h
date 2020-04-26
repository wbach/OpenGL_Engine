#pragma once
#include <memory>

#include "ComponentController.h"
#include "GameEngine/Camera/CameraWrapper.h"
#include "GameEngine/Time/Time.h"

namespace Input
{
class InputManager;
} // namespace Input
namespace GraphicsApi
{
class IGraphicsApi;
} //namespace GraphicsApi
namespace GameEngine
{
class IResourceManager;
class IGpuResourceLoader;

namespace Physics
{
struct IPhysicsApi;
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
    ComponentContext(GraphicsApi::IGraphicsApi& graphicsApi, IGpuResourceLoader& gpuResourceLoader, Time& time, Input::InputManager& input, CameraWrapper& camera, Physics::IPhysicsApi& physicsApi,
                     IResourceManager& resourceManager, Renderer::RenderersManager& renderersManager,
                     ComponentController& componentController)
        : graphicsApi_(graphicsApi)
        , gpuResourceLoader_(gpuResourceLoader)
        , time_(time)
        , inputManager_(input)
        , camera_(camera)
        , physicsApi_(physicsApi)
        , resourceManager_(resourceManager)
        , componentController_(componentController)
        , renderersManager_(renderersManager)
    {
    }

    GraphicsApi::IGraphicsApi& graphicsApi_;
    IGpuResourceLoader& gpuResourceLoader_;
    Time& time_;
    Input::InputManager& inputManager_;
    CameraWrapper& camera_;
    Physics::IPhysicsApi& physicsApi_;
    IResourceManager& resourceManager_;
    ComponentController& componentController_;
    Renderer::RenderersManager& renderersManager_;
    ComponentIdsMap ids_;
};
}  // namespace Components
}  // namespace GameEngine
