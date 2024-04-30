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
class RotateableRunState;

class FollowingState
    : public StateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<InitEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MouseMoveEvent, Utils::StateMachine::TransitionTo<RotateableRunState>>,
          Utils::StateMachine::On<StartAimEvent, Utils::StateMachine::TransitionTo<TransitionState>>>
{
public:
    FollowingState(Context&);

    void onEnter();

    using StateBase::update;
    void update(const InitEvent&);

private:
    bool triggerEventIfMouseMoveDetect();
};
}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
