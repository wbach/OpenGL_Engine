#pragma once
#include <memory>
#include "ComponentController.h"
#include "ComponentsTypes.h"
#include "GameEngine/Time/Time.h"

namespace GameEngine
{
struct ICamera;
class GameObject;
class IResourceManager;

namespace Renderer
{
class RenderersManager;
}

namespace Physics
{
struct IPhysicsApi;
}  // Physics

namespace Components
{
class BaseComponent;

class ComponentFactory
{
public:
    ComponentFactory(ComponentController& componentController, Time& time,
                     std::shared_ptr<IResourceManager>& resourceManager, std::shared_ptr<ICamera>& camera,
                     std::shared_ptr<Physics::IPhysicsApi>* physicsApi);
    std::unique_ptr<BaseComponent> Create(ComponentsType type, GameObject* ptr);
    void SetRendererManager(Renderer::RenderersManager* rendererManager);

private:
    template <class T>
    std::unique_ptr<T> CreateAndBasicInitialize(GameObject* ptr);

private:
    ComponentController& componentController_;
    std::shared_ptr<IResourceManager>& resourceManager_;
    Renderer::RenderersManager* rendererManager_;
    std::shared_ptr<Physics::IPhysicsApi>* physicsApi_;
    std::shared_ptr<ICamera>& camera_;
    Time& time_;
};
}  // Components
}  // GameEngine
