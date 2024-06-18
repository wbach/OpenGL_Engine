#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
class AimState;
class ArmedIdleState;
class DisarmedIdleState;
class ThridPersonCameraComponent;

class AimingStateBase
{
public:
    AimingStateBase(FsmContext&, const std::optional<std::string>&, const std::string&);

    void onEnter(const EndRotationEvent&);
    void onEnter(const EndForwardMoveEvent&);
    void onEnter(const EndBackwardMoveEvent&);
    void update(float);

    void onLeave(const AimStopEvent&);
    void onLeave(const WeaponStateEvent&);
    void onLeave(const SprintStateChangeEvent&);
    void onLeave();

protected:
    void setAnim();
    void stopAnim();
    void unsubscribeAll();

protected:
    FsmContext& context_;

protected:
    std::optional<std::string> jointGroupName_;
    std::vector<IdType> animationSubIds_;

    ThridPersonCameraComponent* thridPersonCameraComponent_;

     std::string animationClip;
};
}  // namespace Components
}  // namespace GameEngine
#pragma once
