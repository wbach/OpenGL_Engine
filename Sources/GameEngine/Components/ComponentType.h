#pragma once
#include <Logger/Log.h>

#include <string>

#if defined(__GNUG__)
#include <cxxabi.h>

#include <cstdlib>
#endif

namespace GameEngine
{
namespace Components
{

template <typename T>
std::string ComponentNameString()
{
#if defined(__GNUG__)
    const char* name = typeid(T).name();
    int status       = 0;
    char* demangled  = abi::__cxa_demangle(name, nullptr, nullptr, &status);
    std::string full = (status == 0 && demangled) ? demangled : name;
    std::free(demangled);
#elif defined(_MSC_VER)
    std::string full = typeid(T).name();  // MSVC juz demangluje czytelnie
#else
    std::string full = typeid(T).name();
#endif

    // wyciecie ostatniego fragmentu po "::"
    auto pos = full.find_last_of(':');
    if (pos != std::string::npos)
        return full.substr(pos + 1);
    return full;
}

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
    static ComponentType type{.id = GetUniqueComponentID(), .name = ComponentNameString<T>()};
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