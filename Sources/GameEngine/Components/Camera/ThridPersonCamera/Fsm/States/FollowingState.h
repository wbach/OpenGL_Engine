#pragma once
#include <Utils/Fsm/Actions.h>

#include "../Context.h"
#include "../ThridPersonCameraEvents.h"
#include "StateBase.h"

namespace GameEngine
{
namespace Components
{
class ThridPersonCameraComponent;

namespace Camera
{
class TransitionState;
class RotateableRunState;

class FollowingState
    : public StateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<InitEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MouseMoveEvent, Utils::StateMachine::TransitionTo<RotateableRunState>>,
          Utils::StateMachine::On<StartAimEvent, Utils::StateMachine::TransitionTo<TransitionState>>>
{
public:
    FollowingState(Context&);
    ~FollowingState();

    void onEnter();
    void update(const InitEvent&);

private:
    void update() override;
    bool triggerEventIfMouseMoveDetect();
};
}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
