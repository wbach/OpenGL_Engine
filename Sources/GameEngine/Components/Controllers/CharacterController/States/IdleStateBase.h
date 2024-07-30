#pragma once
#include "../CharacterControllerEvents.h"

namespace GameEngine
{
namespace Components
{
class JointPoseUpdater;

struct FsmContext;

class IdleStateBase
{
public:
    IdleStateBase(FsmContext&, const std::string&);

    void onEnter(const CrouchChangeStateEvent&);
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
