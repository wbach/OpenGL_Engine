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
    void IncreaseYRotation(Rotation&, float yaw);
    void IncreaseXZRotation(Rotation&, float pitch, const vec3&);
    vec2 calculateMouseMove();
    void LockPitch();
    void LockYaw();

protected:
    FsmContext& context_;
    ThridPersonCameraComponent* thridPersonCameraComponent_;
    const float camSensitive;

    float pitch;
    float yaw;
    vec2 yawLimit;
    vec2 pitchLimit;
};
}  // namespace Components
}  // namespace GameEngine
