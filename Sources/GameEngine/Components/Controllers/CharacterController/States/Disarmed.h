#pragma once
#include <concepts>

namespace GameEngine::Components
{
struct Disarmed
{
};

template <typename T>
concept IsDisarmed = std::derived_from<T, Disarmed>;
}  // namespace GameEngine::Components
