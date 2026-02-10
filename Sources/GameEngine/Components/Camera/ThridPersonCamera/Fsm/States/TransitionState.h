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
class AimState;
class FollowingState;
class RotateableRunState;

class TransitionState
    : public StateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<InitEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MouseInactivityEvent, Utils::StateMachine::TransitionTo<FollowingState>>,
          Utils::StateMachine::On<MouseMoveEvent, Utils::StateMachine::TransitionTo<RotateableRunState>>,
          Utils::StateMachine::On<StopAimEvent, Utils::StateMachine::TransitionTo<FollowingState>>,
          Utils::StateMachine::On<StartAimEvent, Utils::StateMachine::TransitionTo<AimState>>>
{
public:
    TransitionState(Context&);

    void onEnter();
    void onEnter(const StartAimEvent&);
    void onEnter(const StopAimEvent&);
    void onEnter(const MouseInactivityEvent&);

    bool transitionCondition(const StopAimEvent&);
    bool transitionCondition(const StartAimEvent&);
    void update() override;

private:
    float progress;
    float transitionLength;
    Camera::Event processingEvent;

    std::optional<vec4> sourcePosition;
    std::optional<vec4> targetPosition;

    float pitchStart = 0.f;
    float yawStart = 0.f;

    float pitchConversion = 1.f;
};
}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
