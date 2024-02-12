#pragma once
#include <Utils/Fsm/Actions.h>

#include "../Context.h"
#include "../ThridPersonCameraEvents.h"
#include "StateBase.h"

namespace GameEngine
{
namespace Components
{
class ThridPersonCameraComponent;
namespace Camera
{
class TransitionState;

class RotateableRunState
    : public StateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<MouseInactivityEvent, Utils::StateMachine::TransitionTo<TransitionState>>,
          Utils::StateMachine::On<StartAimEvent, Utils::StateMachine::TransitionTo<TransitionState>>>
{
public:
    RotateableRunState(Context&);
    ~RotateableRunState();

    void onEnter();
    void onEnter(const MouseMoveEvent&);

private:
    void update() override;
    vec2 CalcualteMouseMove();
    void lockPitch();
    void updateYaw();
    void mouseControlledCamera(const vec2&);
    void updatePitchYaw(const vec2&);

private:
    vec4 referenceRelativeCamerePosition;

    float mouseSensitivity_;

    vec2 yawLimit;
    vec2 pitchLimit;

    float mouseInactiveTime = 0;
    float mouseInactivityTimer;
};
}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
