#pragma once
#include <concepts>

namespace GameEngine::Components
{
struct Crouch
{
};

template <typename T>
concept IsCrouch = std::derived_from<T, Crouch>;
}  // namespace GameEngine::Components
