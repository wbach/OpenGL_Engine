#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
class ThridPersonCameraComponent;

class AimStateBase
{
public:
    AimStateBase(FsmContext&);
    void onEnter(const AimStartEvent&);
    void onEnter(const EndRotationEvent&);
    void onEnter(const EndForwardMoveEvent&);
    void onEnter(const EndBackwardMoveEvent&);
    void update(float);

    void onLeave(const WeaponStateEvent&);
    void onLeave(const AimStopEvent&);
    void onLeave(const SprintStartEvent&);
    void onLeave(const SprintStateChangeEvent&);

protected:
    void setAnim();
    void stopMultiAnimation();
    void stopAnim();


protected:
    FsmContext& context_;
    ThridPersonCameraComponent* thridPersonCameraComponent_;


};
}  // namespace Components
}  // namespace GameEngine
