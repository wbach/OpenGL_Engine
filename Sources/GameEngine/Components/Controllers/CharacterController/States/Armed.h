#pragma once
#include <concepts>

namespace GameEngine::Components
{
struct Armed
{
};

template <typename T>
concept IsArmed = std::derived_from<T, Armed>;
}  // namespace GameEngine::Components
