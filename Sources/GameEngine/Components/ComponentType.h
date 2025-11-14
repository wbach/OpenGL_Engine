#pragma once
#include <string>
#include <Utils/Utils.h>

namespace GameEngine
{
namespace Components
{
using ComponentTypeID   = std::size_t;
using ComponentTypeName = std::string;

struct ComponentType
{
    ComponentTypeID id{0};
    ComponentTypeName name{};

    bool operator==(const ComponentType& other) const
    {
        return id == other.id;
    }
};

inline const ComponentType NULL_COMPONENT_TYPE{.id = 0u, .name = "global"};

std::ostream& operator<<(std::ostream& os, const ComponentType& type);

ComponentTypeID GetUniqueComponentID();

template <typename T>
inline ComponentType GetComponentType()
{
    static ComponentType type{.id = GetUniqueComponentID(), .name = Utils::GetTypeName<T>()};
    return type;
}
}  // namespace Components
}  // namespace GameEngine

namespace std
{
template <>
struct hash<GameEngine::Components::ComponentType>
{
    std::size_t operator()(const GameEngine::Components::ComponentType& ct) const noexcept
    {
        return std::hash<GameEngine::Components::ComponentTypeID>{}(ct.id);
    }
};
}  // namespace std