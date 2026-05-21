#pragma once
#include <vector>
#include "Types.h"

namespace GameEngine
{
class GameObject;
class NavigationManager;

namespace Components
{
class AIController;
class CharacterController;

struct AIControllerContext
{
    GameObject& gameObject;
    NavigationManager& navigationManager;
    CharacterController& characterController;
    AIController& controller;

    std::vector<vec3> currentPath{};
};
}  // namespace Components
}  // namespace GameEngine
