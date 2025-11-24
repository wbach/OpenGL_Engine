#pragma once
#include <Utils/Fsm/Actions.h>

#include "../Context.h"
#include "../ThridPersonCameraEvents.h"
#include "Fsm/actions/Update.h"
#include "StateBase.h"
#include "Time/Timer.h"

namespace GameEngine
{
namespace Components
{
class ThridPersonCameraComponent;
namespace Camera
{
class TransitionState;

class RotateableRunState : public StateBase,
                           public Utils::StateMachine::Will<
                               Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                               Utils::StateMachine::On<MouseMoveEvent, Utils::StateMachine::Update>,
                               Utils::StateMachine::On<MouseInactivityEvent, Utils::StateMachine::TransitionTo<TransitionState>>,
                               Utils::StateMachine::On<StartAimEvent, Utils::StateMachine::TransitionTo<TransitionState>>>
{
public:
    RotateableRunState(Context&);

    using StateBase::update;
    void update(const MouseMoveEvent&);
    void update() override;

private:
    Utils::Timer timer;
};
}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
