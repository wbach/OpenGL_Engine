#include "ArmedAttackState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
ArmedAttackState::ArmedAttackState(FsmContext &context)
    : AttackStateBase{context, context.animClipNames.armed.attack}
{
}
ArmedAttackState::MaybeAttackStates ArmedAttackState::handle(const MoveForwardEvent &event)
{
    return handleMoveEvents(event);
}

ArmedAttackState::MaybeAttackStates ArmedAttackState::handle(const MoveRightEvent &event)
{
    return handleMoveEvents(event);
}

ArmedAttackState::MaybeAttackStates ArmedAttackState::handle(const MoveLeftEvent &event)
{
    return handleMoveEvents(event);
}

ArmedAttackState::MaybeAttackStates ArmedAttackState::handle(const MoveBackwardEvent &event)
{
    return handleMoveEvents(event);
}

template <typename Event>
ArmedAttackState::MaybeAttackStates ArmedAttackState::handleMoveEvents(const Event &event)
{
    const auto &clips = context.animClipNames.armed.attack;

    if (clips.empty())
    {
        return Utils::StateMachine::Update{};
    }

    if (clips.front().stateType == PlayStateType::idle)
    {
        queue.push_back(event);
        return Utils::StateMachine::Update{};
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
}  // namespace Components
}  // namespace GameEngine
