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
    float transitionLength{0.75f};
};
struct StopAimEvent
{
    float transitionLength{0.75f};
};
struct MouseMoveEvent
{
    vec2i move;
};
struct MouseInactivityEvent
{
    float transitionLength{1.5f};
};

struct StartScriptedMode
{
};

struct StopScriptedMode
{
};

using Event = std::variant<Camera::StartAimEvent, Camera::StopAimEvent, Camera::MouseMoveEvent, Camera::MouseInactivityEvent,
                           StartScriptedMode, StopScriptedMode>;

}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
