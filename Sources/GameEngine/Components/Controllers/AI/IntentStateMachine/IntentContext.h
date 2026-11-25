#pragma once
#include <Types.h>

namespace GameEngine
{
class GameObject;
class NavigationManager;
namespace Components
{
class RoutineComponent;
class AIController;
class CharacterController;

struct IntentContext
{
    RoutineComponent* routineComponent{nullptr};
    GameObject& gameObject;
    AIController& controller;
    NavigationManager& navigationManager;
    CharacterController& characterController;
    std::vector<vec3>& currentPath;
};
}  // namespace Components
}  // namespace GameEngine
