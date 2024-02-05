#include "RotateArmedChangeState.h"

namespace GameEngine
{
namespace Components
{
RotateArmedChangeState::RotateArmedChangeState(FsmContext &context)
    : ArmedChangeStateBase(context, context.upperBodyGroupName)
    , RotateStateBase{context, context.lowerBodyGroupName, context.runSpeed.leftRight,
                      context.animClipNames.disarmed.rotateLeft, context.animClipNames.disarmed.rotateRight}
    , context_{context}
{
}

void RotateArmedChangeState::onEnter(const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
    if (context_.rotateStateData_.rotateSpeed_ > 0.01f)
    {
        setRotateLeftAnim();
    }
    else if (context_.rotateStateData_.rotateSpeed_ < -0.01f)
    {
        setRotateRightAnim();
    }
}

}  // namespace Components
}  // namespace GameEngine
