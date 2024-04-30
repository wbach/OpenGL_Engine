#include "RotatingMoveAndRotateStateBase.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
RotatingMoveAndRotateStateBase::RotatingMoveAndRotateStateBase(
    FsmContext &context, const std::optional<std::string> &moveJointGroupName, const MoveSpeed &moveSpeed,
    const MovmentClipNames &clipnames, const std::string &rotateLeft, const std::string &rotateRight)
    : RotatingMoveState{context, moveJointGroupName, moveSpeed.forward, clipnames.forward}
    , RotateStateBase{context, moveJointGroupName, moveSpeed.rotate, rotateLeft, rotateRight}
{
}

RotatingMoveAndRotateStateBase::RotatingMoveAndRotateStateBase(FsmContext &context,
                                                               const std::optional<std::string> &moveJointGroupName,
                                                               float forwardSpeed, const std::string &forwardAnimName,
                                                               float rotateSpeed, const std::string &rotateLeft,
                                                               const std::string &rotateRight)
    : RotatingMoveState{context, moveJointGroupName, forwardSpeed, forwardAnimName}
    , RotateStateBase{context, moveJointGroupName, rotateSpeed, rotateLeft, rotateRight}
{
}

void RotatingMoveAndRotateStateBase::onEnter(const SprintStateChangeEvent &event)
{
    RotatingMoveState::update(event);
}

void RotatingMoveAndRotateStateBase::onEnter(const MoveForwardEvent &event)
{
    RotatingMoveState::update(event);
}

void RotatingMoveAndRotateStateBase::onEnter(const MoveBackwardEvent &event)
{
    RotatingMoveState::update(event);
}

void RotatingMoveAndRotateStateBase::onEnter(const MoveLeftEvent &event)
{
    RotatingMoveState::onEnter(event);
}

void RotatingMoveAndRotateStateBase::onEnter(const MoveRightEvent &event)
{
    RotatingMoveState::onEnter(event);
}

void RotatingMoveAndRotateStateBase::update(const AttackEvent &event)
{
}

void RotatingMoveAndRotateStateBase::update(const EndAttackEvent &event)
{
}

void RotatingMoveAndRotateStateBase::update(const MoveLeftEvent &e)
{
    RotatingMoveState::update(e);
}

void RotatingMoveAndRotateStateBase::update(const MoveRightEvent &e)
{
    RotatingMoveState::update(e);
}

void RotatingMoveAndRotateStateBase::update(const MoveForwardEvent &e)
{
    RotatingMoveState::update(e);
}

void RotatingMoveAndRotateStateBase::update(const MoveBackwardEvent &e)
{
    RotatingMoveState::update(e);
}

bool RotatingMoveAndRotateStateBase::transitionCondition(const EndForwardMoveEvent &e)
{
    return RotatingMoveState::transitionCondition(e);
}

bool RotatingMoveAndRotateStateBase::transitionCondition(const EndBackwardMoveEvent &e)
{
    return RotatingMoveState::transitionCondition(e);
}

void RotatingMoveAndRotateStateBase::onMoveInactivity()
{
    RotateStateBase::setCurrentAnim();
}

void RotatingMoveAndRotateStateBase::onEnter(const EndJumpEvent &event)
{
    RotatingMoveState::onEnter(event);
}

void RotatingMoveAndRotateStateBase::onEnter(const RotateLeftEvent &event)
{
    RotateStateBase::update(event);
}

void RotatingMoveAndRotateStateBase::onEnter(const RotateRightEvent &event)
{
    RotateStateBase::update(event);
}

void RotatingMoveAndRotateStateBase::onEnter(const RotateTargetEvent &event)
{
    RotateStateBase::update(event);
}

void RotatingMoveAndRotateStateBase::onEnter(const WalkChangeStateEvent &event)
{
    RotatingMoveState::onEnter(event);
}

void RotatingMoveAndRotateStateBase::update(const RotateLeftEvent &event)
{
    RotateStateBase::update(event);
}

void RotatingMoveAndRotateStateBase::update(const RotateRightEvent &event)
{
    RotateStateBase::update(event);
}

void RotatingMoveAndRotateStateBase::update(const RotateTargetEvent &event)
{
    RotateStateBase::update(event);
}

void RotatingMoveAndRotateStateBase::update(float deltaTime)
{
    RotatingMoveState::update(deltaTime);
    RotateStateBase::update(deltaTime);
}

}  // namespace Components
}  // namespace GameEngine
