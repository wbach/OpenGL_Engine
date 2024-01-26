#include "AimRunAndRotateState.h"

#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
AimRunAndRotateState::AimRunAndRotateState(FsmContext& context)
    : AimStateBase(context)
    , MoveStateBase{context, context.runSpeed, context.animClipNames.armed.run.forward,
                    context.animClipNames.armed.run.backward}
    , RotateStateBase(context, context.runSpeed.leftRight, context.animClipNames.armed.rotateLeft,
                      context.animClipNames.armed.rotateRight)
{
}
void AimRunAndRotateState::update(float dt)
{
    MoveStateBase::update(dt);
    RotateStateBase::update(dt);
}

void AimRunAndRotateState::onLeave(const AimStopEvent& event)
{
    AimStateBase::onLeave(event);
}

void AimRunAndRotateState::onLeave(const WeaponStateEvent& event)
{
    AimStateBase::onLeave(event);
}
}  // namespace Components
}  // namespace GameEngine
