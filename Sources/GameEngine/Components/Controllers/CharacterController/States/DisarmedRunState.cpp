#include "DisarmedRunState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
DisarmedRunState::DisarmedRunState(FsmContext &context)
    : RotatingMoveState{context, std::nullopt, context.runSpeed.forward, context.animClipNames.disarmed.movement.run.forward}
{
}
void DisarmedRunState::onMoveInactivity()
{
    context_.animator.ChangeAnimation(context_.animClipNames.disarmed.posture.stand.idle, Animator::AnimationChangeType::smooth,
                                      PlayDirection::forward, jointGroupName_);
}

Utils::StateMachine::Maybe<Utils::StateMachine::TransitionTo<DisarmedAttackState>,
                           Utils::StateMachine::TransitionTo<DisarmedAttackAndWalkState>,
                           Utils::StateMachine::TransitionTo<DisarmedAttackAndRunState>>
DisarmedRunState::handle(const AttackEvent &e)
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

    if (clips.front().stateType == PlayStateType::walk)
    {
        return Utils::StateMachine::TransitionTo<DisarmedAttackAndWalkState>{};
    }

    if (clips.front().stateType == PlayStateType::run)
    {
        return Utils::StateMachine::TransitionTo<DisarmedAttackAndRunState>{};
    }

    return Utils::StateMachine::Nothing{};
}

}  // namespace Components
}  // namespace GameEngine
