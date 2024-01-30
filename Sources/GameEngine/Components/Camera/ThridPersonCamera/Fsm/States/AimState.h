#pragma once
#include <Utils/Fsm/Actions.h>

#include "../Context.h"
#include "../ThridPersonCameraEvents.h"

namespace GameEngine
{
namespace Animation
{
struct Joint;
}
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
    void setJointIfNeeded(const StartAimEvent&);

private:
    Context& context;
    vec4 relativeCamerePosition;
    Animation::Joint* joint;
};
}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
