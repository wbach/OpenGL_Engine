#pragma once
#include <memory>

#include "ComponentContext.h"
#include "IComponentFactory.h"

namespace GameEngine
{
class CameraWrapper;
class GameObject;
class IResourceManager;
class GuiElementFactory;
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
    ComponentFactory(ComponentController&, GraphicsApi::IGraphicsApi&, IGpuResourceLoader&, Time&, Input::InputManager&,
                     IResourceManager&, Renderer::RenderersManager&, CameraWrapper&, Physics::IPhysicsApi&, GuiElementFactory&);
    virtual std::unique_ptr<IComponent> Create(ComponentsType, GameObject&) override;

private:
    ComponentContext context_;
};
}  // namespace Components
}  // namespace GameEngine
