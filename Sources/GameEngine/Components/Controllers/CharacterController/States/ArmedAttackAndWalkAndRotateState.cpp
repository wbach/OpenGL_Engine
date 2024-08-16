#include "ArmedAttackAndWalkAndRotateState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
ArmedAttackAndWalkAndRotateState::ArmedAttackAndWalkAndRotateState(FsmContext& context)
    : AttackStateBase{context, context.animClipNames.armed.attack, context.upperBodyGroupName}
    , RotateStateBase{context, context.lowerBodyGroupName, context.runSpeed.rotate,
                      context.animClipNames.armed.posture.stand.rotate}
    , RotatingMoveState(context, context.lowerBodyGroupName, context.runSpeed.forward,
                        context.animClipNames.armed.movement.walk.forward)
{
}

void ArmedAttackAndWalkAndRotateState::onEnter(const RotateLeftEvent& e)
{
    RotateStateBase::update(e);
}

void ArmedAttackAndWalkAndRotateState::onEnter(const RotateRightEvent& e)
{
    RotateStateBase::update(e);
}

void ArmedAttackAndWalkAndRotateState::onEnter(const RotateTargetEvent& e)
{
    RotateStateBase::update(e);
}
void ArmedAttackAndWalkAndRotateState::update(float v)
{
    AttackStateBase::update(v);
    RotateStateBase::update(v);
    RotatingMoveState::update(v);
}
}  // namespace Components
}  // namespace GameEngine
