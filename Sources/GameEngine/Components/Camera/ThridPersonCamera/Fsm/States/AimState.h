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
class RotateableRunState;
class AimState : public Utils::StateMachine::Will<
                     Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                     Utils::StateMachine::On<StopAimEvent, Utils::StateMachine::TransitionTo<RotateableRunState>>>
{
public:
    AimState(Context&);

    void onEnter(const StartAimEvent&);

private:
    void cameraUpdate();

private:
    Context& context;
    vec4 relativeCamerePosition;
};
}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
