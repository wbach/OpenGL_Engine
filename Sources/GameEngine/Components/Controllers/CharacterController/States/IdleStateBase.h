#pragma once
#include "../CharacterControllerEvents.h"
#include "BaseState.h"

namespace GameEngine
{
namespace Components
{
class JointPoseUpdater;

struct FsmContext;

class IdleStateBase : public BaseState
{
public:
    IdleStateBase(FsmContext&, const std::string&, std::optional<std::string> = std::nullopt);

    using BaseState::update;

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

protected:
    void setIdleAnim();

protected:
    std::string idleAnimName_;
    std::optional<std::string> jointGroupName_;
};
}  // namespace Components
}  // namespace GameEngine
