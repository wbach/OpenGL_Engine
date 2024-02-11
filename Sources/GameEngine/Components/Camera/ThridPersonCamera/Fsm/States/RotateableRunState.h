#pragma once
#include <Utils/Fsm/Actions.h>

#include "../Context.h"
#include "../ThridPersonCameraEvents.h"

namespace GameEngine
{
namespace Components
{
namespace Camera
{
class TransitionState;

class RotateableRunState
    : public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<InitEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<StartAimEvent, Utils::StateMachine::TransitionTo<TransitionState>>>
{
public:
    RotateableRunState(Context&, const vec3& = {-0.5f, 1.0f, -1.5f});
    ~RotateableRunState();

    void onEnter(const StopAimEvent&);
    void update(const InitEvent&);
    const vec4& getRelativeCamerePosition() const;
    const vec4& getLookAtPosition() const;

private:
    void cameraUpdate();
    vec2 CalcualteMouseMove();
    void lockPitch();
    void updateYaw();
    void autoFallowCamera();
    void mouseControlledCamera(const vec2&);

private:
    Context& context;
    vec4 referenceRelativeCamerePosition;
    vec4 relativeCamerePosition;
    vec4 lookAtLocalPosition;
    const mat4 yTranslation;

    float mouseSensitivity_;
    float pitch;
    float yaw;

    vec2 yawLimit;
    vec2 pitchLimit;

    float mouseInactiveTime = 0;
};
}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
