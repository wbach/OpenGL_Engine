#pragma once
#include <Utils/Fsm/Actions.h>

#include <functional>

#include "../FsmContext.h"

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
    IdleStateBase(FsmContext&, const std::string&);

    void onEnter(const EquipEndStateEvent&);
    void onEnter(const DisarmEndStateEvent&);
    void onEnter(const AimStopEvent&);
    void onEnter(const EndForwardMoveEvent&);
    void onEnter(const EndBackwardMoveEvent&);
    void onEnter(const EndRotationEvent&);
    void onEnter(const EndMoveLeftEvent&);
    void onEnter(const EndMoveRightEvent&);
    void onEnter(const EndAttackEvent&);

    void update(float);

private:
    void setIdleAnim();

protected:
    FsmContext& context_;
    std::string idleAnimName_;
};
}  // namespace Components
}  // namespace GameEngine
