#pragma once
#include <Utils/Fsm/Actions.h>

#include "../Context.h"
#include "../ThridPersonCameraEvents.h"
#include "StateBase.h"

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
class TransitionState;
class AimState : public StateBase, public Utils::StateMachine::Will<
                     Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                     Utils::StateMachine::On<StopAimEvent, Utils::StateMachine::TransitionTo<TransitionState>>>
{
public:
    AimState(Context&, const vec3& = {-0.25f, 1.f, -0.75f});

    using StateBase::onEnter;
    void onEnter(const StartAimEvent&);

private:
    void setJointIfNeeded(const StartAimEvent&);

private:
    Animation::Joint* joint;
    vec4 referenceRelativeCamerePosition;
};
}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
