#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
class AimStateBase
{
public:
    AimStateBase(FsmContext&);
    void onEnter();
    void onEnter(const AimStartEvent&);
    void onEnter(const EndRotationEvent&);
    void onEnter(const EndForwardMoveEvent&);
    void onEnter(const EndBackwardMoveEvent&);
    void update(float);

    void onLeave(const WeaponStateEvent&);
    void onLeave(const AimStopEvent&);
    void onLeave();

protected:
    void setAnim();
    void stopMultiAnimation();
    void stopAnim();
    void IncreaseYaw(float yaw);
    void IncreasePitch(float pitch);

protected:
    FsmContext& context_;

    float pitch = 0;
    float yaw = 0;
    Rotation rotation;
};
}  // namespace Components
}  // namespace GameEngine
