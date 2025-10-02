#pragma once
#include <vector>
#include "ComponentType.h"

namespace GameEngine
{
namespace Components
{
using Dependencies = std::vector<ComponentType>;

template <typename... T>
inline Dependencies MakeDependencies()
{
    return {Components::GetComponentType<T>()...};
}
}  // namespace Components
}  // namespace GameEngine