#include "DisarmedRunState.h"

namespace GameEngine
{
namespace Components
{

DisarmedRunState::DisarmedRunState(FsmContext &context)
    : MoveStateBase{context, context.runSpeed, context.animClipNames.disarmed.run.forward,
                    context.animClipNames.disarmed.run.backward}
{
}

void DisarmedRunState::onEnter(const WeaponStateEvent &)
{
    context_.multiAnimations = true;
    StateBase::disarmWeapon();
    MoveStateBase::setCurrentAnim();
}

}  // namespace Components
}  // namespace GameEngine
