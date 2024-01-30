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

    void onEnter(RotateableRunState&, const StartAimEvent&);
    void onEnter(AimState&, const StopAimEvent&);

private:
    void cameraUpdate();

private:
    Context& context;
    ThridPersonCameraComponent* thridPersonCameraComponent;
};
}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
