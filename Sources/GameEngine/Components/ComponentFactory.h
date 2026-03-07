#pragma once
#include <memory>

#include "ComponentContext.h"

class TreeNode;

namespace Utils
{
namespace Time
{
class TimerService;
}
}  // namespace Utils

namespace GameEngine
{
class Scene;
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
class IPhysicsApi;
}  // namespace Physics

namespace Components
{
class ComponentFactory
{
public:
    ComponentFactory(ComponentContext&);

    template <typename Component, typename... Args>
    std::unique_ptr<IComponent> Create(GameObject& gameObject, Args&&... args)
    {
        auto component = std::make_unique<Component>(context_, gameObject, std::forward<Args>(args)...);
        component->Register();
        return component;
    }

    std::unique_ptr<IComponent> Create(const TreeNode&, GameObject&);

private:
    ComponentContext& context_;
};
}  // namespace Components
}  // namespace GameEngine
