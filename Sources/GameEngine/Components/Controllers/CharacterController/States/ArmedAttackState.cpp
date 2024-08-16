#include "ArmedAttackState.h"

#include "../FsmContext.h"
#include "Fsm/actions/Nothing.h"

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
        return Utils::StateMachine::Nothing{};
    }

    if (clips.front().stateType == PlayStateType::idle)
    {
        queue.push_back(event);
    }

    return getCorrespodingState(clips.front().stateType);
}

ArmedAttackState::MaybeAttackStates ArmedAttackState::getCorrespodingState(PlayStateType stateType)
{
    if (stateType == PlayStateType::idle)
    {
        return Utils::StateMachine::Update{};
    }

    if (stateType == PlayStateType::walk)
    {
        return Utils::StateMachine::TransitionTo<ArmedAttackAndWalkState>{};
    }

    if (stateType == PlayStateType::run)
    {
        return Utils::StateMachine::TransitionTo<ArmedAttackAndRunState>{};
    }

    return Utils::StateMachine::Nothing{};
}

ArmedAttackState::MaybeAttackStates ArmedAttackState::handle(const ChangeAnimEvent &event)
{
    if (context.attackStatesContext.nextMoveState == AttackStatesContext::NextMoveState::idle)
        return Utils::StateMachine::Update{};

    return getCorrespodingState(event.stateType);
}

}  // namespace Components
}  // namespace GameEngine
