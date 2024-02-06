#include "WalkArmedChangeState.h"

namespace GameEngine
{
namespace Components
{
WalkArmedChangeState::WalkArmedChangeState(FsmContext &context)
    : ArmedChangeStateBase(context, context.upperBodyGroupName)
    , MoveStateBase{context, context.lowerBodyGroupName, context.walkSpeed, context.animClipNames.disarmed.walk.forward,
                    context.animClipNames.disarmed.walk.backward}
{
}
void WalkArmedChangeState::update(float)
{
}
void WalkArmedChangeState::onEnter(const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::setCurrentAnim();
}
}  // namespace Components
}  // namespace GameEngine
