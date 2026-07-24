#pragma once
#include <Utils/Fsm/Actions.h>

#include "AIStateBase.h"

namespace GameEngine
{
namespace Components
{
class AIAmbientState;

class AIMoveToState : public AIStateBase,
                     public Utils::StateMachine::Will<
                         Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                         Utils::StateMachine::On<MoveToTargetEvent, Utils::StateMachine::Update>,
                         Utils::StateMachine::On<TargetReachedEvent, Utils::StateMachine::TransitionTo<AIAmbientState>>>
{
public:
    AIMoveToState(AIControllerContext&);

    using AIStateBase::onEnter;
    using AIStateBase::onLeave;

    void onEnter(const MoveToTargetEvent&);
    void update(const MoveToTargetEvent&);
    void update(float);
};
}  // namespace Components
}  // namespace GameEngine
