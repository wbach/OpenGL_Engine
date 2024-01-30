#pragma once
#include <string>

namespace Input
{
class InputManager;
}

namespace GameEngine
{
class DisplayManager;
class GameObject;
class CustomCamera;

namespace Components
{
namespace Camera
{
struct Context
{
    Input::InputManager& inputManager;
    DisplayManager& displayManager;
    GameObject& gameObject;
    CustomCamera& camera;
    std::string jointName;
};
}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
