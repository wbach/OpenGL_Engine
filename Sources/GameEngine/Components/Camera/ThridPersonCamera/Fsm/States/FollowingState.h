#pragma once
#include <Utils/Fsm/Actions.h>

#include "../Context.h"
#include "../ThridPersonCameraEvents.h"

namespace GameEngine
{
namespace Components
{
class ThridPersonCameraComponent;

namespace Camera
{
class TransitionState;

class FollowingState
    : public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<InitEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MouseMoveEvent, Utils::StateMachine::TransitionTo<TransitionState>>,
          Utils::StateMachine::On<StartAimEvent, Utils::StateMachine::TransitionTo<TransitionState>>>
{
public:
    FollowingState(Context&, const vec3& = {-0.5f, 1.0f, -1.5f});
    ~FollowingState();

    void onEnter();
    void update(const InitEvent&);

    const vec4& getRelativeCamerePosition() const;
    const vec4& getLookAtPosition() const;

private:
    void cameraUpdate();
    bool triggerEventIfMouseMoveDetect();
    void lockPitch();
    void updateYaw();
    void autoFallowCamera();

private:
    Context& context;
    ThridPersonCameraComponent* thridPersonCameraComponent;
    vec4 referenceRelativeCamerePosition;
    vec4 lookAtLocalPosition;
};
}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
