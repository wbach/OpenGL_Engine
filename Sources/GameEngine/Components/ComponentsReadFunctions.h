#pragma once
#include <Utils/TreeNode.h>

#include <functional>
#include <optional>
#include <unordered_map>

#include "GameEngine/Components/ComponentType.h"
#include "GameEngine/Components/IComponent.h"

namespace GameEngine
{
class GameObject;

namespace Components
{
class IComponent;
struct ComponentContext;

using ComponentReadFunction = std::function<std::unique_ptr<IComponent>(ComponentContext&, const TreeNode&, GameObject&)>;

struct ReadFunctions
{
public:
    using ComponentReadFunctionsType = std::unordered_map<ComponentTypeID, ComponentReadFunction>;
    using ComponentTypeNameToIdType  = std::unordered_map<ComponentTypeName, ComponentTypeID>;
    static ReadFunctions& instance();

    ComponentReadFunction get(const ComponentTypeName&);
    bool isRegistered(const ComponentTypeName&);

    const ComponentTypeNameToIdType& getComponentTypeNameToId() const
    {
        return typeNameToId;
    }

    const ComponentReadFunctionsType& getComponentReadFunctions() const
    {
        return componentsReadFunctions;
    }

    void RegisterComponent(const ComponentType&, ComponentReadFunction);
    void UnregisterComponent(const ComponentTypeName&);
    std::optional<ComponentTypeID> getComponentTypeIdByName(const ComponentTypeName&);

private:
    ComponentReadFunctionsType componentsReadFunctions;
    ComponentTypeNameToIdType typeNameToId;
};

void regsiterComponentReadFunction(const ComponentType&, ComponentReadFunction);
void unregsiterComponentReadFunction(const ComponentTypeName&);

std::optional<ComponentTypeID> getComponentTypeIdByName(const ComponentTypeName&);
extern const std::string CSTR_TYPE;
}  // namespace Components
}  // namespace GameEngine
