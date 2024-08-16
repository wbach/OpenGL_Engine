#include "DisarmedAttackAndWalkAndRotateState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
DisarmedAttackAndWalkAndRotateState::DisarmedAttackAndWalkAndRotateState(FsmContext& context)
    : AttackStateBase{context, context.animClipNames.disarmed.attack, context.upperBodyGroupName}
    , RotateStateBase{context, context.lowerBodyGroupName, context.runSpeed.rotate,
                      context.animClipNames.disarmed.posture.stand.rotate}
    , RotatingMoveState(context, context.lowerBodyGroupName, context.runSpeed.forward,
                        context.animClipNames.disarmed.movement.walk.forward)
{
}

void DisarmedAttackAndWalkAndRotateState::onEnter(const RotateLeftEvent& e)
{
    RotateStateBase::update(e);
}

void DisarmedAttackAndWalkAndRotateState::onEnter(const RotateRightEvent& e)
{
    RotateStateBase::update(e);
}

void DisarmedAttackAndWalkAndRotateState::onEnter(const RotateTargetEvent& e)
{
    RotateStateBase::update(e);
}
void DisarmedAttackAndWalkAndRotateState::update(float v)
{
    AttackStateBase::update(v);
    RotateStateBase::update(v);
    RotatingMoveState::update(v);
}
}  // namespace Components
}  // namespace GameEngine
