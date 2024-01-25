#pragma once
#include <Utils/Fsm/Actions.h>

#include "DrawArrowStateBase.h"
#include "RotateStateBase.h"

namespace GameEngine
{
namespace Components
{
class AimState;
class AimRotateState;
class ArmedRotateState;
class DrawArrowState;
class DisarmedRotateState;

class DrawArrowRotateState
    : public DrawArrowStateBase,
      public RotateStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<DisarmedRotateState>>,
          Utils::StateMachine::On<EndRotationEvent, Utils::StateMachine::TransitionTo<DrawArrowState>>,
          Utils::StateMachine::On<AimStartEvent, Utils::StateMachine::TransitionTo<AimRotateState>>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::TransitionTo<ArmedRotateState>>>
{
public:
    DrawArrowRotateState(FsmContext&);
    void onEnter(const DrawArrowEvent&);
    void onEnter(const RotateLeftEvent&);
    void onEnter(const RotateRightEvent&);

    void update(const RotateLeftEvent&);
    void update(const RotateRightEvent&);
    void update(float);

private:
    FsmContext& context_;
};
}  // namespace Components
}  // namespace GameEngine
#pragma once
