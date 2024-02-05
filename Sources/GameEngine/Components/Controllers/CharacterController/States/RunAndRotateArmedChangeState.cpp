#include "RunAndRotateArmedChangeState.h"

namespace GameEngine
{
namespace Components
{

RunAndRotateArmedChangeState::RunAndRotateArmedChangeState(FsmContext &context)
    : MoveAndRotateStateBase{context, context.runSpeed, context.animClipNames.disarmed.run,
                             context.animClipNames.disarmed.rotateLeft, context.animClipNames.disarmed.rotateRight}
    , context_{context}
{
}
void RunAndRotateArmedChangeState::onEnter(const WeaponStateEvent &)
{
    context_.multiAnimations = true;
    MoveStateBase::disarmWeapon();
    MoveStateBase::setCurrentAnim();
}
}
}  // namespace GameEngine
