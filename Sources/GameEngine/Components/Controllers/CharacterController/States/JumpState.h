#pragma once
#include <optional>
#include <vector>

#include "BaseState.h"
#include "CharacterControllerCommonDefs.h"

namespace GameEngine
{
namespace Components
{
class JumpState : public BaseState,
                  public Utils::StateMachine::Will<
                      Utils::StateMachine::ByDefault<Utils::StateMachine::Queue>,
                      Utils::StateMachine::On<JumpEvent, Utils::StateMachine::Nothing>,
                      Utils::StateMachine::On<JumpConfirmEvent, Utils::StateMachine::Update>,
                      Utils::StateMachine::On<StartFallingEvent, Utils::StateMachine::TransitionTo<DisarmedFallingState>>,
                      Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
                      Utils::StateMachine::On<GroundDetectionEvent, Utils::StateMachine::BackToPreviousState>>
{
public:
    JumpState(FsmContext&);

    void onEnter(const JumpEvent&);
    void update(float);
    void update(const JumpConfirmEvent&);
    void onLeave(const GroundDetectionEvent&);

private:
    void setAnim();

private:
    std::optional<float> jumpAttemptTimer;
};
}  // namespace Components
}  // namespace GameEngine
