#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
class DrawArrowState;
class ArmedIdleState;
class DisarmedIdleState;
class ThridPersonCameraComponent;

class RecoilStateBase
{
public:
    RecoilStateBase(FsmContext&);
    void onEnter(const EndRotationEvent&);
    void onEnter(const AttackEvent&);
    void onEnter(const EndForwardMoveEvent&);
    void onEnter(const EndBackwardMoveEvent&);
    void update(float);

    void onLeave(const AimStopEvent&);
    void onLeave(const WeaponStateEvent&);
    void onLeave(const SprintStartEvent&);
    void onLeave(const SprintStateChangeEvent&);

protected:
    void setAnim();
    void stopMultiAnimation();
    void stopAnim();

protected:
    FsmContext& context_;
    std::string animName_;
    ThridPersonCameraComponent* thridPersonCameraComponent_;
};
}  // namespace Components
}  // namespace GameEngine
