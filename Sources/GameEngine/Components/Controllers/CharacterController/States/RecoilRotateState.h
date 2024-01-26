#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"
#include "RecoilStateBase.h"
#include "RotateStateBase.h"

namespace GameEngine
{
namespace Components
{
class AimState;
class RecoilState;
class ArmedRotateState;
class DisarmedRotateState;
class DrawArrowRotateState;

class RecoilRotateState
    : public RecoilStateBase,
      public RotateStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<DisarmedRotateState>>,
          Utils::StateMachine::On<EndRotationEvent, Utils::StateMachine::TransitionTo<RecoilState>>,
          Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::TransitionTo<DrawArrowRotateState>>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::TransitionTo<ArmedRotateState>>>
{
public:
    RecoilRotateState(FsmContext&);

    using RecoilStateBase::onEnter;

    void onEnter(const RotateLeftEvent&);
    void onEnter(const RotateRightEvent&);

    void update(const RotateLeftEvent&);
    void update(const RotateRightEvent&);
    void update(float);

    void leave();

private:
    FsmContext& context_;
};
}  // namespace Components
}  // namespace GameEngine
