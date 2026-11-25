#pragma once
#include <Types.h>
namespace GameEngine
{
class GameObject;
class NavigationManager;
namespace Components
{
class AIController;
class CharacterController;
struct ReactionStateContext
{
    GameObject& gameObject;
    AIController& controller;
    NavigationManager& navigationManager;
    CharacterController& characterController;
    std::vector<vec3>& currentPath;
};
}  // namespace Components
}  // namespace GameEngine
