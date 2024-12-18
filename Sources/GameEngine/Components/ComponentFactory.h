#pragma once
#include <Utils/TreeNode.h>
#include <Utils/Time/TimerService.h>

#include <functional>
#include <memory>
#include <unordered_map>

#include "ComponentContext.h"

namespace GameEngine
{
class Scene;
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
class ComponentFactory
{
public:
    ComponentFactory(Scene&, ComponentController&, GraphicsApi::IGraphicsApi&, IGpuResourceLoader&, Time&,
                     Input::InputManager&, IResourceManager&, Renderer::RenderersManager&, CameraWrapper&,
                     Physics::IPhysicsApi&, GuiElementFactory&, Utils::Time::TimerService&);

    template <typename Component, typename... Args>
    std::unique_ptr<IComponent> Create(GameObject& gameObject, Args && ... args)
    {
        return std::make_unique<Component>(context_, gameObject, std::forward<Args>(args)...);
    }

    std::unique_ptr<IComponent> Create(const TreeNode&, GameObject&);

private:
    ComponentContext context_;
};
}  // namespace Components
}  // namespace GameEngine
