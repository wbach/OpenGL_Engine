#pragma once
#include <memory>
#include "ComponentController.h"
#include "GameEngine/Time/Time.h"

namespace GameEngine
{
struct ICamera;
class GameObject;
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
    ComponentContext(Time& time, ICamera& camera, GameObject& gameObject, Physics::IPhysicsApi& physicsApi,
                     IResourceManager& resourceManager, Renderer::RenderersManager& renderersManager,
                     ComponentController& componentController, ComponentIdsMap idsMap)
        : time_(time)
        , camera_(camera)
        , thisObject_(gameObject)
        , physicsApi_(physicsApi)
        , resourceManager_(resourceManager)
        , renderersManager_(renderersManager)
        , componentController_(componentController)
        , ids_(idsMap)
    {
    }

    Time& time_;
    ICamera& camera_;
    GameObject& thisObject_;
    Physics::IPhysicsApi& physicsApi_;
    IResourceManager& resourceManager_;
    ComponentController& componentController_;
    Renderer::RenderersManager& renderersManager_;
    ComponentIdsMap ids_;
};
}  // namespace Components
}  // namespace GameEngine
