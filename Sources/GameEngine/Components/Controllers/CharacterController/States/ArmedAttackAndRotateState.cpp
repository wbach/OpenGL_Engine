#include "ArmedAttackAndRotateState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
ArmedAttackAndRotateState::ArmedAttackAndRotateState(FsmContext &context)
    : AttackStateBase{context, context.animClipNames.armed.attack, context.upperBodyGroupName}
    , RotateStateBase{context, context.lowerBodyGroupName, context.runSpeed.rotate,
                      context.animClipNames.armed.posture.stand.rotate}
    , context{context}
{
}

void ArmedAttackAndRotateState::onEnter(const EndForwardMoveEvent &e)
{
    RotateStateBase::onEnter(e);
    subscribe();
}

void ArmedAttackAndRotateState::onEnter(const EndMoveRightEvent &e)
{
    RotateStateBase::onEnter(e);
    subscribe();
}

void ArmedAttackAndRotateState::onEnter(const EndMoveLeftEvent &e)
{
    RotateStateBase::onEnter(e);
    subscribe();
}

void ArmedAttackAndRotateState::onEnter(const EndBackwardMoveEvent &e)
{
    RotateStateBase::onEnter(e);
    subscribe();
}
void ArmedAttackAndRotateState::update(float v)
{
    AttackStateBase::update(v);
    RotateStateBase::update(v);
}

ArmedAttackAndRotateState::MaybeAttackStates ArmedAttackAndRotateState::handle(const ChangeAnimEvent &event)
{
    if (context.attackStatesContext.nextMoveState == AttackStatesContext::NextMoveState::idle or
        event.stateType == PlayStateType::idle)
    {
        return Utils::StateMachine::Update{};
    }

    if (event.stateType == PlayStateType::walk)
    {
        DEBUG_LOG("PlayStateType::walk");
        return Utils::StateMachine::TransitionTo<ArmedAttackAndWalkAndRotateState>{};
    }

    if (event.stateType == PlayStateType::run)
    {
        DEBUG_LOG("PlayStateType::run");
        return Utils::StateMachine::TransitionTo<ArmedAttackAndRunAndRotateState>{};
    }

    return Utils::StateMachine::Nothing{};
}
}  // namespace Components
}  // namespace GameEngine
