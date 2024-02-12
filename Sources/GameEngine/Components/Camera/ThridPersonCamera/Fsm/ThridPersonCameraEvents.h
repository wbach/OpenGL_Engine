#pragma once
#include <string>
#include <variant>
#include <GameEngine/Animations/JointId.h>

namespace GameEngine
{
namespace Components
{
namespace Camera
{
struct InitEvent
{
};
struct StartAimEvent
{
    Animation::JointId jointId;
};
struct StopAimEvent
{
};
struct MouseMoveEvent
{
    vec2ui move;
};
struct MouseInactivityEvent
{
};

using Event = std::variant<Camera::StartAimEvent, Camera::StopAimEvent, Camera::MouseMoveEvent, Camera::MouseInactivityEvent>;

}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
