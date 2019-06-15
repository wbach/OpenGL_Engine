#pragma once
#include <memory>
#include "ComponentContext.h"
#include "IComponentFactory.h"

namespace GameEngine
{
class CameraWrapper;
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
}  // namespace Physics

namespace Components
{
class ComponentFactory : public IComponentFactory
{
public:
    ComponentFactory(ComponentController& componentController, Time& time, Input::InputManager& input,
                     IResourceManager& resourceManager, Renderer::RenderersManager& rendererManager,
                     CameraWrapper& camera, Physics::IPhysicsApi& physicsApi);
    virtual std::unique_ptr<IComponent> Create(ComponentsType type, GameObject& ptr) override;

private:
    ComponentContext context_;
};
}  // namespace Components
}  // namespace GameEngine
