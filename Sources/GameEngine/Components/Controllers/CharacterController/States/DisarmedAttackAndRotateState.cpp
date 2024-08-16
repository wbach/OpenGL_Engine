#include "DisarmedAttackAndRotateState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
DisarmedAttackAndRotateState::DisarmedAttackAndRotateState(FsmContext &context)
    : AttackStateBase{context, context.animClipNames.disarmed.attack, context.upperBodyGroupName}
    , RotateStateBase{context, context.lowerBodyGroupName, context.runSpeed.rotate,
                      context.animClipNames.disarmed.posture.stand.rotate}
{
}

void DisarmedAttackAndRotateState::onEnter(const EndForwardMoveEvent &e)
{
    RotateStateBase::onEnter(e);
    subscribe();
}

void DisarmedAttackAndRotateState::onEnter(const EndMoveRightEvent &e)
{
    RotateStateBase::onEnter(e);
    subscribe();
}

void DisarmedAttackAndRotateState::onEnter(const EndMoveLeftEvent &e)
{
    RotateStateBase::onEnter(e);
    subscribe();
}

void DisarmedAttackAndRotateState::onEnter(const EndBackwardMoveEvent &e)
{
    RotateStateBase::onEnter(e);
    subscribe();
}
void DisarmedAttackAndRotateState::update(float v)
{
    AttackStateBase::update(v);
    RotateStateBase::update(v);
}

DisarmedAttackAndRotateState::MaybeAttackStates DisarmedAttackAndRotateState::handle(const ChangeAnimEvent &event)
{
    if (context.attackStatesContext.nextMoveState == AttackStatesContext::NextMoveState::idle or
        event.stateType == PlayStateType::idle)
    {
        return Utils::StateMachine::Update{};
    }

    if (event.stateType == PlayStateType::walk)
    {
        DEBUG_LOG("PlayStateType::walk");
        return Utils::StateMachine::TransitionTo<DisarmedAttackAndWalkAndRotateState>{};
    }

    if (event.stateType == PlayStateType::run)
    {
        DEBUG_LOG("PlayStateType::run");
        return Utils::StateMachine::TransitionTo<DisarmedAttackAndRunAndRotateState>{};
    }

    return Utils::StateMachine::Nothing{};
}
}  // namespace Components
}  // namespace GameEngine
