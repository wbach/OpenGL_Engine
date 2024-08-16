#include "ArmedRunState.h"

#include "../CharacterController.h"
#include "../FsmContext.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
ArmedRunState::ArmedRunState(FsmContext &context)
    : RotatingMoveState{context, std::nullopt, context.runSpeed.forward, context.animClipNames.armed.movement.run.forward}
{
}

Utils::StateMachine::Maybe<Utils::StateMachine::TransitionTo<ArmedAttackState>,
                           Utils::StateMachine::TransitionTo<ArmedAttackAndWalkState>,
                           Utils::StateMachine::TransitionTo<ArmedAttackAndRunState> >
ArmedRunState::handle(const AttackEvent &)
{
    const auto &clips = context_.animClipNames.armed.attack;

    if (clips.empty())
    {
        DEBUG_LOG("x");
        return Utils::StateMachine::Nothing{};
    }

    if (clips.front().stateType == PlayStateType::idle)
    {
        DEBUG_LOG("idle");
        return Utils::StateMachine::TransitionTo<ArmedAttackState>{};
    }

    if (clips.front().stateType == PlayStateType::walk)
    {
        DEBUG_LOG("walk");
        return Utils::StateMachine::TransitionTo<ArmedAttackAndWalkState>{};
    }

    if (clips.front().stateType == PlayStateType::run)
    {
        DEBUG_LOG("run");
        return Utils::StateMachine::TransitionTo<ArmedAttackAndRunState>{};
    }

    return Utils::StateMachine::Nothing{};
}

void ArmedRunState::onMoveInactivity()
{
    context_.animator.ChangeAnimation(context_.animClipNames.armed.posture.stand.idle, Animator::AnimationChangeType::smooth,
                                      PlayDirection::forward, jointGroupName_);
}
}  // namespace Components
}  // namespace GameEngine
