#include "MoveAndRotateStateBase.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
MoveAndRotateStateBase::MoveAndRotateStateBase(FsmContext &context, const std::optional<std::string> &moveJointGroupName,
                                               const MoveSpeed &moveSpeed, const MovmentClipNames &clipnames,
                                               const RotateAnimationClips &rotateAnimationClips)
    : MoveStateBase{context, moveJointGroupName, moveSpeed, clipnames}
    , RotateStateBase{context, moveJointGroupName, moveSpeed.rotate, rotateAnimationClips}
{
}

MoveAndRotateStateBase::MoveAndRotateStateBase(FsmContext &context, const std::optional<std::string> &moveJointGroupName,
                                               float forwardSpeed, const std::string &forwardAnimName, float rotateSpeed,
                                               const RotateAnimationClips &rotateAnimationClips)
    : MoveStateBase{context, moveJointGroupName, forwardSpeed, forwardAnimName}
    , RotateStateBase{context, moveJointGroupName, rotateSpeed, rotateAnimationClips}
{
}

void MoveAndRotateStateBase::onEnter(const SprintStateChangeEvent &event)
{
    MoveStateBase::update(event);
}

void MoveAndRotateStateBase::onEnter(const MoveForwardEvent &event)
{
    MoveStateBase::update(event);
}

void MoveAndRotateStateBase::onEnter(const MoveBackwardEvent &event)
{
    MoveStateBase::update(event);
}

void MoveAndRotateStateBase::onEnter(const MoveLeftEvent &event)
{
    MoveStateBase::onEnter(event);
}

void MoveAndRotateStateBase::onEnter(const MoveRightEvent &event)
{
    MoveStateBase::onEnter(event);
}

void MoveAndRotateStateBase::update(const AttackEvent &event)
{
}

void MoveAndRotateStateBase::update(const EndAttackEvent &event)
{
}

void MoveAndRotateStateBase::update(const MoveLeftEvent &e)
{
    MoveStateBase::update(e);
}

void MoveAndRotateStateBase::update(const MoveRightEvent &e)
{
    MoveStateBase::update(e);
}

void MoveAndRotateStateBase::update(const MoveForwardEvent &e)
{
    MoveStateBase::update(e);
}

void MoveAndRotateStateBase::update(const MoveBackwardEvent &e)
{
    MoveStateBase::update(e);
}

bool MoveAndRotateStateBase::transitionCondition(const EndForwardMoveEvent &e)
{
    return MoveStateBase::transitionCondition(e);
}

bool MoveAndRotateStateBase::transitionCondition(const EndBackwardMoveEvent &e)
{
    return MoveStateBase::transitionCondition(e);
}

void MoveAndRotateStateBase::onMoveInactivity()
{
    RotateStateBase::setCurrentAnim();
}

void MoveAndRotateStateBase::onEnter(const GroundDetectionEvent &event)
{
    MoveStateBase::onEnter(event);
}

void MoveAndRotateStateBase::onEnter(const RotateLeftEvent &event)
{
    RotateStateBase::update(event);
}

void MoveAndRotateStateBase::onEnter(const RotateRightEvent &event)
{
    RotateStateBase::update(event);
}

void MoveAndRotateStateBase::onEnter(const RotateTargetEvent &event)
{
    RotateStateBase::update(event);
}

void MoveAndRotateStateBase::onEnter(const WalkChangeStateEvent &event)
{
    MoveStateBase::onEnter(event);
}

void MoveAndRotateStateBase::update(const RotateLeftEvent &event)
{
    RotateStateBase::update(event);
}

void MoveAndRotateStateBase::update(const RotateRightEvent &event)
{
    RotateStateBase::update(event);
}

void MoveAndRotateStateBase::update(const RotateTargetEvent &event)
{
    RotateStateBase::update(event);
}

void MoveAndRotateStateBase::update(float deltaTime)
{
    MoveStateBase::update(deltaTime);
    RotateStateBase::update(deltaTime);
}

}  // namespace Components
}  // namespace GameEngine
