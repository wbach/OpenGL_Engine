#include "AimRunState.h"

#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
AimRunState::AimRunState(FsmContext& context)
    : AimStateBase(context)
    , MoveStateBase{context, context.runSpeed, context.animClipNames.armed.run.forward,
                    context.animClipNames.armed.run.backward}
    , context_{context}
{
}

void AimRunState::onEnter()
{
    DEBUG_LOG("onEnter()");
    context_.multiAnimations = true;
}

void AimRunState::onEnter(const AimStartEvent& event)
{
    AimStateBase::onEnter(event);
}

void AimRunState::onLeave(const WeaponStateEvent& event)
{
    AimStateBase::onLeave(event);
}

void AimRunState::onLeave(const AimStopEvent& event)
{
    AimStateBase::onLeave(event);
}
}  // namespace Components
}  // namespace GameEngine
