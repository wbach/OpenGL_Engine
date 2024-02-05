#include "RotateArmedChangeState.h"

namespace GameEngine
{
namespace Components
{

RotateArmedChangeState::RotateArmedChangeState(FsmContext &context)
    : RotateStateBase{context, context.runSpeed.leftRight, context.animClipNames.disarmed.rotateLeft,
                      context.animClipNames.disarmed.rotateRight}
{
}

void RotateArmedChangeState::onEnter(const WeaponStateEvent &)
{
    context_.multiAnimations = true;
    StateBase::disarmWeapon();
    if (context_.rotateStateData_.rotateSpeed_ > 0.01f)
    {
        setRotateLeftAnim();
    }
    else if (context_.rotateStateData_.rotateSpeed_ < -0.01f)
    {
        setRotateRightAnim();
    }
}

}
}  // namespace GameEngine
