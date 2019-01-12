#pragma once
#include "IComponentFactory.h"
#include "ComponentContext.h"
#include <memory>

namespace GameEngine
{
struct ICamera;
class GameObject;
class IResourceManager;
struct Time;

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

class ComponentFactory : public IComponentFactory
{
public:
    ComponentFactory(ComponentController& componentController, Time& time,
                     IResourceManager& resourceManager, Renderer::RenderersManager& rendererManager, std::unique_ptr<ICamera>& camera,
                     Physics::IPhysicsApi& physicsApi);
    virtual std::unique_ptr<IComponent> Create(ComponentsType type, GameObject& ptr) override;

private:
    ComponentContext context_;
};
}  // Components
}  // GameEngine
