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
    static ReadFunctions& instance();
    std::unordered_map<std::string, ComponentReadFunction> componentsReadFunctions;
};

void regsiterComponentReadFunction(const std::string&, ComponentReadFunction);
extern const std::string CSTR_TYPE;
}  // namespace Components
}  // namespace GameEngine
