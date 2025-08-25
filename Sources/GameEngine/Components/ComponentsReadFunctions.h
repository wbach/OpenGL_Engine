#pragma once
#include <Utils/TreeNode.h>

#include <functional>
#include <unordered_map>

namespace GameEngine
{
class GameObject;

namespace Components
{
class IComponent;
struct ComponentContext;

using ComponentReadFunction =
    std::function<std::unique_ptr<IComponent>(ComponentContext&, const TreeNode&, GameObject&)>;

struct ReadFunctions
{
public:
    using ComponentName = std::string;
    static ReadFunctions& instance();
    std::unordered_map<ComponentName, ComponentReadFunction> componentsReadFunctions;
};

void regsiterComponentReadFunction(const ReadFunctions::ComponentName&, ComponentReadFunction);
extern const std::string CSTR_TYPE;
}  // namespace Components
}  // namespace GameEngine
