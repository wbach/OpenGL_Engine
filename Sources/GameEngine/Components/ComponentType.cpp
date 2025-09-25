#include "ComponentType.h"
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
std::ostream& operator<<(std::ostream& os, const ComponentType& type)
{
    os << "ComponentType{id=" << type.id << ", name=\"" << type.name << "\"}";
    return os;
}
}  // namespace Components
}  // namespace GameEngine
