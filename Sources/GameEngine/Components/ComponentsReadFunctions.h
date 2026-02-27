#pragma once
#include <functional>
#include <optional>
#include <unordered_map>
#include <memory>

#include "GameEngine/Components/ComponentType.h"

#pragma once

class TreeNode;

namespace GameEngine
{
class GameObject;

namespace Components
{
class IComponent;
struct ComponentContext;

using ComponentReadFunction = std::function<std::unique_ptr<IComponent>(ComponentContext&, const TreeNode&, GameObject&)>;

struct ENGINE_API ReadFunctions
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

ENGINE_API void regsiterComponentReadFunction(const ComponentType&, ComponentReadFunction);
ENGINE_API void unregsiterComponentReadFunction(const ComponentTypeName&);

std::optional<ComponentTypeID> getComponentTypeIdByName(const ComponentTypeName&);
ENGINE_API extern const std::string CSTR_TYPE;
}  // namespace Components
}  // namespace GameEngine
