#include "ArmedAttackAndWalkState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
ArmedAttackAndWalkState::ArmedAttackAndWalkState(FsmContext& context)
    : AttackStateBase{context, context.animClipNames.armed.attack, context.upperBodyGroupName}
    , RotatingMoveState(context, context.lowerBodyGroupName, context.walkSpeed.forward,
                        context.animClipNames.armed.walk.forward)
{
}

void ArmedAttackAndWalkState::update(float v)
{
    AttackStateBase::update(v);
    RotatingMoveState::update(v);
}
void ArmedAttackAndWalkState::onMoveInactivity()
{
    context_.animator.StopAnimation(context.lowerBodyGroupName);
}
ArmedAttackAndWalkState::MaybeAttackStates ArmedAttackAndWalkState::handle(const ChangeAnimEvent& event)
{
    if (event.stateType == PlayStateType::idle)
    {
        return Utils::StateMachine::TransitionTo<ArmedAttackState>{};
    }

    if (event.stateType == PlayStateType::run or event.stateType == PlayStateType::walk)
    {
        return Utils::StateMachine::Update{};
    }

    return Utils::StateMachine::Nothing{};
}
}  // namespace Components
}  // namespace GameEngine
