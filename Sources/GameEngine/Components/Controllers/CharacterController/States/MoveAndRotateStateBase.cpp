#include "MoveAndRotateStateBase.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
MoveAndRotateStateBase::MoveAndRotateStateBase(FsmContext &context,
                                               const std::optional<std::string> &moveJointGroupName,
                                               const MoveSpeed &moveSpeed, const MovmentClipNames &clipnames,
                                               const std::string &rotateLeft, const std::string &rotateRight)
    : MoveStateBase{context, moveJointGroupName, moveSpeed, clipnames.forward, clipnames.backward}
    , RotateStateBase{context, std::nullopt, moveSpeed.leftRight, rotateLeft, rotateRight}
{
}

MoveAndRotateStateBase::MoveAndRotateStateBase(FsmContext &context,
                                               const std::optional<std::string> &moveJointGroupName, float forwardSpeed,
                                               const std::string &forwardAnimName, float rotateSpeed,
                                               const std::string &rotateLeft, const std::string &rotateRight)
    : MoveStateBase{context, moveJointGroupName, forwardSpeed, forwardAnimName}
    , RotateStateBase{context, std::nullopt, rotateSpeed, rotateLeft, rotateRight}
{
}

void MoveAndRotateStateBase::onEnter(const EquipEndStateEvent &event)
{
    MoveStateBase::onEnter(event);
}

void MoveAndRotateStateBase::onEnter(const DisarmEndStateEvent& event)
{
    MoveStateBase::onEnter(event);
}

void MoveAndRotateStateBase::onEnter(const SprintStartEvent &event)
{
    MoveStateBase::update(event);
}

void MoveAndRotateStateBase::onEnter(const SprintStateChangeEvent &event)
{
    MoveStateBase::update(event);
}

void MoveAndRotateStateBase::onEnter(const WalkForwardEvent &event)
{
    MoveStateBase::update(event);
}

void MoveAndRotateStateBase::onEnter(const WalkBackwardEvent &event)
{
    MoveStateBase::update(event);
}

void MoveAndRotateStateBase::update(const AttackEvent &event)
{
}

void MoveAndRotateStateBase::update(const EndAttackEvent &event)
{
}

void MoveAndRotateStateBase::update(const RunForwardEvent &e)
{
    MoveStateBase::update(e);
}

void MoveAndRotateStateBase::update(const RunBackwardEvent &e)
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

bool MoveAndRotateStateBase::transitionCondition(const SprintStartEvent &e)
{
    return MoveStateBase::transitionCondition(e);
}

bool MoveAndRotateStateBase::transitionCondition(const SprintStateChangeEvent &e)
{
    return MoveStateBase::transitionCondition(e);
}

void MoveAndRotateStateBase::onEnter(const EndJumpEvent &event)
{
    MoveStateBase::onEnter(event);
}

void MoveAndRotateStateBase::onEnter(const RunForwardEvent &event)
{
    MoveStateBase::onEnter(event);
}

void MoveAndRotateStateBase::onEnter(const RunBackwardEvent &event)
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
