#pragma once
#include "ComponentCore.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"

namespace GameEngine
{
namespace Components
{
template <typename T>
class Component : public ComponentCore
{
protected:
    Component(ComponentContext& ctx, GameObject& obj)
        : ComponentCore(GetComponentType<T>(), ctx, obj)
    {
    }

public:
    static void registerReadFunctions()
    {
        auto readFunc = [](ComponentContext& ctx, const TreeNode& node, GameObject& obj)
        {
            auto component = std::make_unique<T>(ctx, obj);
            component->read(node);
            return component;
        };
        regsiterComponentReadFunction(GetComponentType<T>(), readFunc);
    }
};

#define DECLARE_COMPONENT(ClassName) class ENGINE_API ClassName : public Component<ClassName>

}  // namespace Components
}  // namespace GameEngine
