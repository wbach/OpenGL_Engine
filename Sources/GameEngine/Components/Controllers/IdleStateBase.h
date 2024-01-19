#pragma once
#include <Utils/Fsm/Actions.h>

#include <functional>

#include "FsmContext.h"
#include "StateBase.h"

namespace GameEngine
{
namespace Components
{
class DisarmedRunState;
class DisarmedRotateState;
class DisarmedWalkState;
class DisarmedSprintState;
class DisarmedCrouchState;

class ArmedIdleState;

class JumpState;
class DeathState;
class JointPoseUpdater;

class IdleStateBase : public StateBase
{
public:
    IdleStateBase(FsmContext&, const std::string&);

    void onEnter(const EndForwardMoveEvent&);
    void onEnter(const EndRotationEvent&);

    void update(const AttackEvent&);
    void update(const EndAttackEvent&);
    void update(const WeaponChangeEndEvent&);
    void update(float);

private:
    void setIdleAnim();

private:
    std::string idleAnimName_;
};
}  // namespace Components
}  // namespace GameEngine
