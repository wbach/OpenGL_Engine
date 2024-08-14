#include "ArmedAttackAndRunAndRotateState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
ArmedAttackAndRunAndRotateState::ArmedAttackAndRunAndRotateState(FsmContext& context)
    : AttackStateBase{context, context.animClipNames.armed.attack, context.upperBodyGroupName}
    , RotateStateBase{context, context.lowerBodyGroupName, context.runSpeed.rotate,
                      context.animClipNames.armed.rotateLeft, context.animClipNames.armed.rotateRight}
    , RotatingMoveState(context, context.lowerBodyGroupName, context.runSpeed.forward,
                        context.animClipNames.armed.run.forward)
{
}

void ArmedAttackAndRunAndRotateState::onEnter(const RotateLeftEvent& e)
{
    RotateStateBase::update(e);
}

void ArmedAttackAndRunAndRotateState::onEnter(const RotateRightEvent& e)
{
    RotateStateBase::update(e);
}

void ArmedAttackAndRunAndRotateState::onEnter(const RotateTargetEvent& e)
{
    RotateStateBase::update(e);
}
void ArmedAttackAndRunAndRotateState::update(float v)
{
    AttackStateBase::update(v);
    RotateStateBase::update(v);
    RotatingMoveState::update(v);
}
}  // namespace Components
}  // namespace GameEngine
