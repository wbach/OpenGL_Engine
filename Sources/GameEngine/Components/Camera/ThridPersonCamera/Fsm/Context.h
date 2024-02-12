#pragma once
#include <Types.h>

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

    struct LocalCameraPosition
    {
        vec3 run;
        vec3 aim;
    };

    LocalCameraPosition cameraPositions;

    float pitch{0};
    float yaw{0};
};
}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
