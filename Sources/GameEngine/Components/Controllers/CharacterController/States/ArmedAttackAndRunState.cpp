#include "ArmedAttackAndRunState.h"
#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
ArmedAttackAndRunState::ArmedAttackAndRunState(FsmContext & context)
    : AttackStateBase{context, context.animClipNames.armed.attack, context.upperBodyGroupName}
    , RotatingMoveState(context, context.lowerBodyGroupName, context.runSpeed.forward, context.animClipNames.armed.run.forward)
{
}
void ArmedAttackAndRunState::update(float v)
{
    AttackStateBase::update(v);
    RotatingMoveState::update(v);
}
void ArmedAttackAndRunState::onMoveInactivity()
{
    context_.animator.StopAnimation(context.lowerBodyGroupName);
}
}  // namespace Components
}  // namespace GameEngine
