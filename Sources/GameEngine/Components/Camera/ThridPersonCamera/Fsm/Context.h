#pragma once
#include <Types.h>

#include "Common/Transform.h"
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
class CameraComponent;
namespace Camera
{
struct Context
{
    common::TransformContext lookAtTransformContext;
    Input::InputManager& inputManager;
    DisplayManager& displayManager;
    GameObject& gameObject;
    CameraComponent& camera;
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
