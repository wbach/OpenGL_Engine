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
    vec3 anchorPosition_{0.f};
    Quaternion anchorRotation_;

    std::vector<vec3> currentPath{};
};

}  // namespace Components
}  // namespace GameEngine
