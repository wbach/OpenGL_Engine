#include "ArmedAttackAndRunState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
ArmedAttackAndRunState::ArmedAttackAndRunState(FsmContext& context)
    : AttackStateBase{context, context.animClipNames.armed.attack, context.upperBodyGroupName}
    , RotatingMoveState(context, context.lowerBodyGroupName, context.runSpeed.forward,
                        context.animClipNames.armed.movement.run.forward)
{
}
void ArmedAttackAndRunState::update(float v)
{
    AttackStateBase::update(v);
    RotatingMoveState::update(v);
}
void ArmedAttackAndRunState::onMoveInactivity()
{
    context_.animator.StopAnimation(context.lowerBodyGroupName);
}

ArmedAttackAndRunState::MaybeAttackStates ArmedAttackAndRunState::handle(const ChangeAnimEvent& event)
{
    if (event.stateType == PlayStateType::idle)
    {
        DEBUG_LOG("PlayStateType::idle");
        return Utils::StateMachine::TransitionTo<ArmedAttackState>{};
    }

    if (event.stateType == PlayStateType::walk)
    {
        DEBUG_LOG("PlayStateType::walk");
        return Utils::StateMachine::TransitionTo<ArmedAttackAndWalkState>{};
    }

    if (event.stateType == PlayStateType::run)
    {
        DEBUG_LOG("PlayStateType::run");
        return Utils::StateMachine::Update{};
    }

    return Utils::StateMachine::Nothing{};
}
}  // namespace Components
}  // namespace GameEngine
