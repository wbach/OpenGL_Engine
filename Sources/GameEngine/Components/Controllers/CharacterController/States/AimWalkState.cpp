#include "AimWalkState.h"

#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
AimWalkState::AimWalkState(FsmContext& context)
    : AimStateBase(context)
    , MoveStateBase{context, context.walkSpeed, context.animClipNames.armed.walk.forward,
                    context.animClipNames.armed.walk.backward}
    , context_{context}
{
}

void AimWalkState::onEnter(const WalkForwardEvent & event)
{
    context_.multiAnimations = true;
    AimStateBase::setAnim();
    MoveStateBase::onEnter(event);
}

void AimWalkState::onEnter(const WalkBackwardEvent & event)
{
    context_.multiAnimations = true;
    AimStateBase::setAnim();
    MoveStateBase::onEnter(event);
}

void AimWalkState::onEnter(const WalkChangeStateEvent & event)
{
    context_.multiAnimations = true;
    AimStateBase::setAnim();
    MoveStateBase::onEnter(event);
}
void AimWalkState::update(float)
{
}
}  // namespace Components
}  // namespace GameEngine
