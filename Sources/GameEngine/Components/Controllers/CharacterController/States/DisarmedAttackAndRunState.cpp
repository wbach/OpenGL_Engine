#include "DisarmedAttackAndRunState.h"

#include "../FsmContext.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
DisarmedAttackAndRunState::DisarmedAttackAndRunState(FsmContext& context)
    : AttackStateBase{context, context.animClipNames.disarmed.attack, context.upperBodyGroupName}
    , RotatingMoveState(context, context.lowerBodyGroupName, context.runSpeed.forward,
                        context.animClipNames.disarmed.run.forward)
{
}
DisarmedAttackAndRunState::MaybeAttackStates DisarmedAttackAndRunState::handle(const ChangeAnimEvent& event)
{
    if (event.stateType == PlayStateType::idle)
    {
        DEBUG_LOG("PlayStateType::idle");
        return Utils::StateMachine::TransitionTo<DisarmedAttackState>{};
    }

    if (event.stateType == PlayStateType::walk)
    {
        DEBUG_LOG("PlayStateType::walk");
        return Utils::StateMachine::TransitionTo<DisarmedAttackAndWalkState>{};
    }

    if (event.stateType == PlayStateType::run)
    {
        DEBUG_LOG("PlayStateType::run");
        return Utils::StateMachine::Update{};
    }

    return Utils::StateMachine::Nothing{};
}
void DisarmedAttackAndRunState::update(float v)
{
    AttackStateBase::update(v);
    RotatingMoveState::update(v);
}
void DisarmedAttackAndRunState::onMoveInactivity()
{
    context_.animator.StopAnimation(context.lowerBodyGroupName);
}
}  // namespace Components
}  // namespace GameEngine
