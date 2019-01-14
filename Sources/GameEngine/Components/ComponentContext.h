#pragma once
#include <memory>
#include "ComponentController.h"
#include "GameEngine/Time/Time.h"

namespace GameEngine
{
struct ICamera;
class IResourceManager;

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
    ComponentContext(Time& time, std::unique_ptr<ICamera>& camera, Physics::IPhysicsApi& physicsApi,
                     IResourceManager& resourceManager, Renderer::RenderersManager& renderersManager,
                     ComponentController& componentController)
        : time_(time)
        , camera_(camera)
        , physicsApi_(physicsApi)
        , resourceManager_(resourceManager)
        , componentController_(componentController)
        , renderersManager_(renderersManager)
    {
    }

    Time& time_;
    std::unique_ptr<ICamera>& camera_;
    Physics::IPhysicsApi& physicsApi_;
    IResourceManager& resourceManager_;
    ComponentController& componentController_;
    Renderer::RenderersManager& renderersManager_;
    ComponentIdsMap ids_;
};
}  // namespace Components
}  // namespace GameEngine
