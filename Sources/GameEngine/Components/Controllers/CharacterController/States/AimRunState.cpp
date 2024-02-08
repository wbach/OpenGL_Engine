#include "AimRunState.h"

#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
AimRunState::AimRunState(FsmContext& context)
    : AimStateBase(context, context.upperBodyGroupName)
    , MoveStateBase{context, context.lowerBodyGroupName, context.runSpeed, context.animClipNames.armed.run.forward,
                    context.animClipNames.armed.run.backward}
    , context_{context}
{
}

void AimRunState::onEnter(const AimStartEvent& event)
{
    AimStateBase::onEnter(event);
}

void AimRunState::update(float dt)
{
    MoveStateBase::update(dt);
    AimStateBase::update(dt);
}

void AimRunState::onLeave(const WeaponStateEvent& event)
{
    AimStateBase::onLeave(event);
}

void AimRunState::onLeave(const AimStopEvent& event)
{
    AimStateBase::onLeave(event);
}

void AimRunState::onLeave(const SprintStateChangeEvent& event)
{
    AimStateBase::onLeave(event);
}
}  // namespace Components
}  // namespace GameEngine
