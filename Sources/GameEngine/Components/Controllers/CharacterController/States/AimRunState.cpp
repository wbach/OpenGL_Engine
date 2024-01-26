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

void AimRunState::onEnter(const RunForwardEvent& event)
{
    DEBUG_LOG("onEnter(RunForwardEvent)");
    MoveStateBase::onEnter(event);
}

void AimRunState::onLeave(const AimStopEvent&)
{
    context_.multiAnimations = false;
}
void AimRunState::update(float dt)
{
    MoveStateBase::update(dt);
}
}  // namespace Components
}  // namespace GameEngine
