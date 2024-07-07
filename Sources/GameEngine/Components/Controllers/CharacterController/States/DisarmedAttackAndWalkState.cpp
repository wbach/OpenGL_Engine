#include "DisarmedAttackAndWalkState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
DisarmedAttackAndWalkState::DisarmedAttackAndWalkState(FsmContext& context)
    : AttackStateBase{context, context.animClipNames.disarmed.attack, context.upperBodyGroupName}
    , RotatingMoveState(context, context.lowerBodyGroupName, context.walkSpeed.forward,
                        context.animClipNames.disarmed.walk.forward)
{
}

void DisarmedAttackAndWalkState::update(float v)
{
    AttackStateBase::update(v);
    RotatingMoveState::update(v);
}
void DisarmedAttackAndWalkState::onMoveInactivity()
{
    context_.animator.StopAnimation(context.lowerBodyGroupName);
}
}  // namespace Components
}  // namespace GameEngine
