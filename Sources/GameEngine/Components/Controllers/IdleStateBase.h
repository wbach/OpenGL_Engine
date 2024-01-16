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

    using StateBase::onEnter;

    void update(const AttackEvent&);
    void update(const EndAttackEvent&);
    void update(float);
    void onLeave();

protected:
    void enter() override;
    void onWeaponChanged() override;

private:
    FsmContext& context_;
    std::string idleAnimName_;
};
}  // namespace Components
}  // namespace GameEngine
