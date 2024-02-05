#include "WalkArmedChangeState.h"

namespace GameEngine
{
namespace Components
{
WalkArmedChangeState::WalkArmedChangeState(FsmContext &context)
    : MoveStateBase{context, context.walkSpeed, context.animClipNames.disarmed.walk.forward,
                    context.animClipNames.disarmed.walk.backward}
{
}
void WalkArmedChangeState::onEnter(const WeaponStateEvent &)
{
    context_.multiAnimations = true;
    StateBase::disarmWeapon();
    MoveStateBase::setCurrentAnim();
}
}  // namespace Components
}  // namespace GameEngine
