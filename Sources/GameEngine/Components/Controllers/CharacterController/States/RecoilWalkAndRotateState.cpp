#include "RecoilWalkAndRotateState.h"

#include "../FsmContext.h"
#include "../CharacterControllerEvents.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
RecoilWalkAndRotateState::RecoilWalkAndRotateState(FsmContext &context)
    : RecoilStateBase(context, context.upperBodyGroupName)
    , MoveAndRotateStateBase{context, context.lowerBodyGroupName, context.walkSpeed, context.animClipNames.armed.movement.walk,
                             context.animClipNames.armed.posture.stand.rotate}
{
}

void RecoilWalkAndRotateState::onEnter(const AttackEvent &event)
{
    RecoilStateBase::onEnter(event);
}

void RecoilWalkAndRotateState::update(float dt)
{
    MoveStateBase::update(dt);
    RotateStateBase::update(dt);
    RecoilStateBase::update(dt);
}

void RecoilWalkAndRotateState::onLeave(const AimStopEvent &e)
{
    RecoilStateBase::onLeave(e);
}

void RecoilWalkAndRotateState::onLeave(const WeaponStateEvent &e)
{
    RecoilStateBase::onLeave(e);
}

void RecoilWalkAndRotateState::onLeave(const SprintStateChangeEvent &e)
{
    RecoilStateBase::onLeave(e);
}

void RecoilWalkAndRotateState::onMoveInactivity()
{
    RotateStateBase::setCurrentAnim();
}
}  // namespace Components
}  // namespace GameEngine
