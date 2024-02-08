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

class DrawArrowStateBase
{
public:
    DrawArrowStateBase(FsmContext&, const std::optional<std::string>&);
    void onEnter(const DrawArrowEvent&);
    void onEnter(const ReloadArrowEvent&);
    void onEnter(const EndRotationEvent&);
    void onEnter(const EndForwardMoveEvent&);
    void onEnter(const EndBackwardMoveEvent&);
    void update(float);

    void onLeave(const AimStopEvent&);
    void onLeave(const WeaponStateEvent&);
    void onLeave(const SprintStartEvent&);
    void onLeave(const SprintStateChangeEvent&);

private:
    void setAnim();
    void stopAnim();

protected:
    FsmContext& context_;

private:
    std::optional<std::string> jointGroupName_;
    std::optional<IdType> subId_;

    ThridPersonCameraComponent* thridPersonCameraComponent_;
};
}  // namespace Components
}  // namespace GameEngine
#pragma once
