#include "ArmedRunState.h"

#include "../CharacterController.h"
#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
ArmedRunState::ArmedRunState(FsmContext &context)
    : RotatingMoveState{context, std::nullopt, context.runSpeed.forward, context.animClipNames.armed.run.forward}
{
}

Utils::StateMachine::Maybe<Utils::StateMachine::TransitionTo<ArmedAttackState>,
                           Utils::StateMachine::TransitionTo<ArmedAttackAndWalkState>,
                           Utils::StateMachine::TransitionTo<ArmedAttackAndRunState> >
ArmedRunState::handle(const AttackEvent &)
{
    const auto &clips = context_.animClipNames.disarmed.attack;

    if (clips.empty())
    {
        return Utils::StateMachine::Nothing{};
    }

    if (clips.front().stateType == PlayStateType::idle)
    {
        return Utils::StateMachine::TransitionTo<ArmedAttackState>{};
    }

    if (clips.front().stateType == PlayStateType::walk)
    {
        return Utils::StateMachine::TransitionTo<ArmedAttackAndWalkState>{};
    }

    if (clips.front().stateType == PlayStateType::run)
    {
        return Utils::StateMachine::TransitionTo<ArmedAttackAndRunState>{};
    }

    return Utils::StateMachine::Nothing{};
}

void ArmedRunState::onMoveInactivity()
{
    context_.animator.ChangeAnimation(context_.animClipNames.armed.idle, Animator::AnimationChangeType::smooth,
                                      PlayDirection::forward, jointGroupName_);
}
}  // namespace Components
}  // namespace GameEngine
