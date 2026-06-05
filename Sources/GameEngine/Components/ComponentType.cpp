#include "ComponentType.h"

#include <mutex>
#include <ostream>

namespace GameEngine
{
namespace Components
{
ComponentTypeID GetUniqueComponentID()
{
    static ComponentTypeID lastID = 1;
    return lastID++;
}
ComponentType GetOrCreateComponentType(const std::string& typeName)
{
    static std::unordered_map<std::string, ComponentType> registry;
    static std::mutex registryMutex;

    std::lock_guard<std::mutex> lock(registryMutex);

    auto it = registry.find(typeName);
    if (it != registry.end())
    {
        return it->second;
    }

    ComponentType newType{.id = GetUniqueComponentID(), .name = typeName};

    registry[typeName] = newType;
    return newType;
}
std::ostream& operator<<(std::ostream& os, const ComponentType& type)
{
    os << "ComponentType{id=" << type.id << ", name=\"" << type.name << "\"}";
    return os;
}
}  // namespace Components
}  // namespace GameEngine
