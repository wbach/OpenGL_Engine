#include "DisarmedAttackAndRunAndRotateState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
DisarmedAttackAndRunAndRotateState::DisarmedAttackAndRunAndRotateState(FsmContext& context)
    : AttackStateBase{context, context.animClipNames.disarmed.attack, context.upperBodyGroupName}
    , RotateStateBase{context, context.lowerBodyGroupName, context.runSpeed.rotate,
                      context.animClipNames.disarmed.posture.stand.rotate}
    , RotatingMoveState(context, context.lowerBodyGroupName, context.runSpeed.forward,
                        context.animClipNames.disarmed.movement.run.forward)
{
}

void DisarmedAttackAndRunAndRotateState::onEnter(const RotateLeftEvent& e)
{
    RotateStateBase::update(e);
}

void DisarmedAttackAndRunAndRotateState::onEnter(const RotateRightEvent& e)
{
    RotateStateBase::update(e);
}

void DisarmedAttackAndRunAndRotateState::onEnter(const RotateTargetEvent& e)
{
    RotateStateBase::update(e);
}
void DisarmedAttackAndRunAndRotateState::update(float v)
{
    AttackStateBase::update(v);
    RotateStateBase::update(v);
    RotatingMoveState::update(v);
}
}  // namespace Components
}  // namespace GameEngine
