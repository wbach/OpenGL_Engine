#include "DisarmedAttackState.h"

#include <Logger/Log.h>

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
DisarmedAttackState::DisarmedAttackState(FsmContext &context)
    : AttackStateBase{context, context.animClipNames.disarmed.attack}
{
}

DisarmedAttackState::MaybeAttackStates DisarmedAttackState::handle(const MoveForwardEvent &event)
{
    return handleMoveEvents(event);
}

DisarmedAttackState::MaybeAttackStates DisarmedAttackState::handle(const MoveRightEvent &event)
{
    return handleMoveEvents(event);
}

DisarmedAttackState::MaybeAttackStates DisarmedAttackState::handle(const MoveLeftEvent &event)
{
    return handleMoveEvents(event);
}

DisarmedAttackState::MaybeAttackStates DisarmedAttackState::handle(const MoveBackwardEvent &event)
{
    return handleMoveEvents(event);
}

template <typename Event>
DisarmedAttackState::MaybeAttackStates DisarmedAttackState::handleMoveEvents(const Event &event)
{
    const auto &clips = context_.animClipNames.disarmed.attack;

    if (clips.empty())
    {
        return Utils::StateMachine::Update{};
    }

    if (clips.front().stateType == PlayStateType::idle)
    {
        queue_.push_back(event);
    }

    return getCorrespodingState(clips.front().stateType);
}
DisarmedAttackState::MaybeAttackStates DisarmedAttackState::getCorrespodingState(PlayStateType stateType)
{
    if (stateType == PlayStateType::idle)
    {
        return Utils::StateMachine::Update{};
    }

    if (stateType == PlayStateType::walk)
    {
        return Utils::StateMachine::TransitionTo<DisarmedAttackAndWalkState>{};
    }

    if (stateType == PlayStateType::run)
    {
        return Utils::StateMachine::TransitionTo<DisarmedAttackAndRunState>{};
    }

    return Utils::StateMachine::Nothing{};
}

DisarmedAttackState::MaybeAttackStates DisarmedAttackState::handle(const ChangeAnimEvent &event)
{
    if (context_.attackStatesContext.nextMoveState == AttackStatesContext::NextMoveState::idle)
        return Utils::StateMachine::Update{};

    return getCorrespodingState(event.stateType);
}
}  // namespace Components
}  // namespace GameEngine
