#include "DisarmedWalkState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
DisarmedWalkState::DisarmedWalkState(FsmContext &context)
    : RotatingMoveState{context, std::nullopt, context.walkSpeed.forward, context.animClipNames.disarmed.movement.walk.forward}
{
}

Utils::StateMachine::Maybe<Utils::StateMachine::TransitionTo<DisarmedAttackState>,
                           Utils::StateMachine::TransitionTo<DisarmedAttackAndWalkState> >
DisarmedWalkState::handle(const AttackEvent &)
{
    const auto &clips = context_.animClipNames.disarmed.attack;

    if (clips.empty())
    {
        return Utils::StateMachine::Nothing{};
    }

    if (clips.front().stateType == PlayStateType::idle)
    {
        return Utils::StateMachine::TransitionTo<DisarmedAttackState>{};
    }

    if (clips.front().stateType == PlayStateType::walk or clips.front().stateType == PlayStateType::run)
    {
        return Utils::StateMachine::TransitionTo<DisarmedAttackAndWalkState>{};
    }

    return Utils::StateMachine::Nothing{};
}

void DisarmedWalkState::onMoveInactivity()
{
    setAnim(context_.animClipNames.disarmed.posture.stand.idle);
}
}  // namespace Components
}  // namespace GameEngine
