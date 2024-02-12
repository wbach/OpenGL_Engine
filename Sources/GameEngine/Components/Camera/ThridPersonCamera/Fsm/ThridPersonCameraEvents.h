#pragma once
#include <GameEngine/Animations/JointId.h>

#include <string>
#include <variant>

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
    float transitionLength{0.5f};
};
struct StopAimEvent
{
    float transitionLength{0.5f};
};
struct MouseMoveEvent
{
    vec2ui move;
};
struct MouseInactivityEvent
{
    mat4 yTranslation{mat4(1.f)};
    float pitch{0};
    float yaw{0};
    float transitionLength{0.5f};
    vec4 relativeCamerePosition;
    vec4 lookAtLocalPosition;
};

using Event =
    std::variant<Camera::StartAimEvent, Camera::StopAimEvent, Camera::MouseMoveEvent, Camera::MouseInactivityEvent>;

}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
