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
};
}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
