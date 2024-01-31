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
class AimState;
class RotateableRunState;

class TransitionState
    : public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<InitEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<StopAimEvent, Utils::StateMachine::TransitionTo<RotateableRunState>>,
          Utils::StateMachine::On<StartAimEvent, Utils::StateMachine::TransitionTo<AimState>>>
{
public:
    TransitionState(Context&);

    void onEnter();
    void onEnter(RotateableRunState&, const StartAimEvent&);
    void onEnter(AimState&, const StopAimEvent&);

private:
    void cameraUpdate();
    void calculateLookAts();

private:
    Context& context;
    ThridPersonCameraComponent* thridPersonCameraComponent;
    float progress;
    float transitionLength;
    using Event = std::variant<Camera::StartAimEvent, Camera::StopAimEvent>;
    Event processingEvent;

    vec4 sourcePosition;
    vec4 sourceLookAt;
    vec4 targetPosition;
    vec4 targetLookAt;
};
}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
