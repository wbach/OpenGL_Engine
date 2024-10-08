#include "RecoilWalkState.h"

#include "../FsmContext.h"
#include "Logger/Log.h"


namespace GameEngine
{
namespace Components
{
RecoilWalkState::RecoilWalkState(FsmContext& context)
    : RecoilStateBase(context, context.upperBodyGroupName)
    , MoveStateBase{context, context.lowerBodyGroupName, context.walkSpeed, context.animClipNames.armed.movement.walk}
    , context_{context}
{
}

void RecoilWalkState::onEnter(const AttackEvent& event)
{
    RecoilStateBase::onEnter(event);
}
void RecoilWalkState::update(float dt)
{
    MoveStateBase::update(dt);
    RecoilStateBase::update(dt);
}

void RecoilWalkState::onMoveInactivity()
{
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}
void RecoilWalkState::onLeave(const AimStopEvent& e)
{
    RecoilStateBase::onLeave(e);
}

void RecoilWalkState::onLeave(const SprintStateChangeEvent& e)
{
    RecoilStateBase::onLeave(e);
}

void RecoilWalkState::onLeave(const WeaponStateEvent& e)
{
    RecoilStateBase::onLeave(e);
}
}  // namespace Components
}  // namespace GameEngine
