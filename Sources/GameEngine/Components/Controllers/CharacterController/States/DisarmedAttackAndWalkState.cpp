#include "DisarmedAttackAndWalkState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
DisarmedAttackAndWalkState::DisarmedAttackAndWalkState(FsmContext& context)
    : AttackStateBase{context, context.animClipNames.disarmed.attack, context.upperBodyGroupName}
    , RotatingMoveState(context, context.lowerBodyGroupName, context.walkSpeed.forward,
                        context.animClipNames.disarmed.movement.walk.forward)
    , context{context}
{
}

DisarmedAttackAndWalkState::MaybeAttackStates DisarmedAttackAndWalkState::handle(const ChangeAnimEvent& event)
{
    if (event.stateType == PlayStateType::idle)
    {
        return Utils::StateMachine::TransitionTo<DisarmedAttackState>{};
    }

    if (event.stateType == PlayStateType::run or event.stateType == PlayStateType::walk)
    {
        return Utils::StateMachine::Update{};
    }

    return Utils::StateMachine::Nothing{};
}

void DisarmedAttackAndWalkState::update(float v)
{
    AttackStateBase::update(v);
    RotatingMoveState::update(v);
}
void DisarmedAttackAndWalkState::onMoveInactivity()
{
    context.animator.StopAnimation(context.lowerBodyGroupName);
}
}  // namespace Components
}  // namespace GameEngine
