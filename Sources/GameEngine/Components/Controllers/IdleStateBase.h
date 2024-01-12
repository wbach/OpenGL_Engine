#pragma once
#include <Utils/Fsm/Actions.h>

#include <functional>

#include "FsmContext.h"

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

class IdleStateBase
{
public:
    IdleStateBase(FsmContext&, float, const std::string&, const std::string&);

    void onEnter();
    void onEnter(const WeaponStateEvent&);
    void update(const AttackEvent&);
    void update(const EndAttackEvent&);
    void update(float);
    void onLeave();

protected:
    virtual void setWeaponPosition() = 0;

private:
    void unsubscribe();

private:
    FsmContext& context_;
    std::string idleAnimName_;
    std::string armChangeAnimName_;
    std::optional<uint32> subscribeForTransitionAnimationEnd_;
    bool weaponChangeTriggered_{false};
    float armChangeTimeStamp_{0.0};

    std::function<void()> setWeaponPosition_;
};
}  // namespace Components
}  // namespace GameEngine
