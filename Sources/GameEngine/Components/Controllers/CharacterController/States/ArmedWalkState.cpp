#include "ArmedWalkState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
ArmedWalkState::ArmedWalkState(FsmContext &context)
    : RotatingMoveState{context, std::nullopt, context.walkSpeed.forward, context.animClipNames.armed.walk.forward}
{
}

Utils::StateMachine::Maybe<Utils::StateMachine::TransitionTo<ArmedAttackState>,
                           Utils::StateMachine::TransitionTo<ArmedAttackAndWalkState> >
ArmedWalkState::handle(const AttackEvent &)
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

    if (clips.front().stateType == PlayStateType::walk or
        clips.front().stateType == PlayStateType::run)
    {
        return Utils::StateMachine::TransitionTo<ArmedAttackAndWalkState>{};
    }

    return Utils::StateMachine::Nothing{};
}
void ArmedWalkState::onMoveInactivity()
{
    setAnim(context_.animClipNames.armed.idle);
}
}  // namespace Components
}  // namespace GameEngine
