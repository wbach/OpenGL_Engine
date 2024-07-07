#include "DisarmedAttackAndRunState.h"
#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
DisarmedAttackAndRunState::DisarmedAttackAndRunState(FsmContext & context)
    : AttackStateBase{context, context.animClipNames.disarmed.attack, context.upperBodyGroupName}
    , RotatingMoveState(context, context.lowerBodyGroupName, context.runSpeed.forward, context.animClipNames.disarmed.run.forward)
{
}
void DisarmedAttackAndRunState::update(float v)
{
    AttackStateBase::update(v);
    RotatingMoveState::update(v);
}
void DisarmedAttackAndRunState::onMoveInactivity()
{
    context_.animator.StopAnimation(context.lowerBodyGroupName);
}
}  // namespace Components
}  // namespace GameEngine
