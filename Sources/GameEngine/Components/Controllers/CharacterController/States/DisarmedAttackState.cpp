#include "DisarmedAttackState.h"

#include "../FsmContext.h"
#include <Logger/Log.h>

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
    const auto &clips = context.animClipNames.disarmed.attack;

    if (clips.empty())
    {
        return Utils::StateMachine::Update{};
    }

    if (clips.front().stateType == AttackAnimation::PlayStateType::idle)
    {
        queue.push_back(event);
        return Utils::StateMachine::Update{};
    }

    if (clips.front().stateType == AttackAnimation::PlayStateType::walk)
    {
        return Utils::StateMachine::TransitionTo<DisarmedAttackAndWalkState>{};
    }

    if (clips.front().stateType == AttackAnimation::PlayStateType::run)
    {
        return Utils::StateMachine::TransitionTo<DisarmedAttackAndRunState>{};
    }

    return Utils::StateMachine::Nothing{};
}
}  // namespace Components
}  // namespace GameEngine
