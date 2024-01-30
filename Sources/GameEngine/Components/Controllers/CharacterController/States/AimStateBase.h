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
    AimStateBase(FsmContext&, const std::string& = "mixamorig:Spine1");
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

protected:
    FsmContext& context_;
    ThridPersonCameraComponent* thridPersonCameraComponent_;
    Animation::Joint* joint_;
    const float camSensitive = 0.2f;
};
}  // namespace Components
}  // namespace GameEngine
