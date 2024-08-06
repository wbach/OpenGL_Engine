#include "ArmedAttackAndWalkState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
ArmedAttackAndWalkState::ArmedAttackAndWalkState(FsmContext& context)
    : AttackStateBase{context, context.animClipNames.armed.attack, context.upperBodyGroupName}
    , RotatingMoveState(context, context.lowerBodyGroupName, context.walkSpeed.forward,
                        context.animClipNames.armed.walk.forward)
{
}

void ArmedAttackAndWalkState::update(float v)
{
    AttackStateBase::update(v);
    RotatingMoveState::update(v);
}
void ArmedAttackAndWalkState::onMoveInactivity()
{
    context_.animator.StopAnimation(context.lowerBodyGroupName);
}
}  // namespace Components
}  // namespace GameEngine
