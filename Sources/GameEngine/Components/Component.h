#pragma once
#include "ComponentCore.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "TreeNode.h"

namespace GameEngine
{
namespace Components
{
template <typename T, typename Base = ComponentCore>
class Component : public Base
{
protected:
    Component(ComponentContext& ctx, GameObject& obj)
        : Base(GetComponentType<T>(), ctx, obj)
    {
    }

public:
    static void registerReadFunctions()
    {
        auto readFunc = [](ComponentContext& ctx, const TreeNode& node, GameObject& obj)
        {
            auto component = std::make_unique<T>(ctx, obj);
            component->readFromNode(node);
            return component;
        };
        regsiterComponentReadFunction(GetComponentType<T>(), readFunc);
    }
};

#define GET_MACRO(_1, _2, NAME, ...) NAME
#define DECLARE_COMPONENT_1(ClassName) class ENGINE_API ClassName : public Component<ClassName>
#define DECLARE_COMPONENT_2(ClassName, BaseClass) class ENGINE_API ClassName : public Component<ClassName, BaseClass>
#define DECLARE_COMPONENT(...) GET_MACRO(__VA_ARGS__, DECLARE_COMPONENT_2, DECLARE_COMPONENT_1)(__VA_ARGS__)

#define REGISTER_COMPONENT(ClassName)         \
    static bool ClassName##_registered = []() \
    {                                         \
        ClassName::registerReadFunctions();   \
        return true;                          \
    }();
}  // namespace Components
}  // namespace GameEngine
