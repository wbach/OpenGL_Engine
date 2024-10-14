#pragma once
#include "BaseState.h"
#include "CharacterControllerCommonDefs.h"

#include <optional>
#include <vector>

namespace GameEngine
{
namespace Components
{
class JumpState : public BaseState,
                  public Utils::StateMachine::Will<
                      Utils::StateMachine::ByDefault<Utils::StateMachine::Queue>,
                      Utils::StateMachine::On<JumpEvent, Utils::StateMachine::Nothing>,
                      Utils::StateMachine::On<StartFallingEvent, Utils::StateMachine::TransitionTo<DisarmedFallingState>>,
                      Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
                      Utils::StateMachine::On<GroundDetectionEvent, Utils::StateMachine::BackToPreviousState>>
{
public:
    JumpState(FsmContext&);

    void onEnter(const JumpEvent&);
    void onLeave(const GroundDetectionEvent&);
};
}  // namespace Components
}  // namespace GameEngine
