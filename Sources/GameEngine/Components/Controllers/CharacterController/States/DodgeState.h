#pragma once
#include <optional>
#include <vector>

#include "BaseState.h"
#include "CharacterControllerCommonDefs.h"

namespace GameEngine
{
namespace Components
{
class DodgeState : public BaseState,
                  public Utils::StateMachine::Will<
                      Utils::StateMachine::ByDefault<Utils::StateMachine::Queue>,
                      Utils::StateMachine::On<DodgeDiveEvent, Utils::StateMachine::Nothing>,
                      Utils::StateMachine::On<StartFallingEvent, Utils::StateMachine::TransitionTo<DisarmedFallingState>>,
                      Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
                      Utils::StateMachine::On<DodgeEndEvent, Utils::StateMachine::BackToPreviousState>>
{
public:
    DodgeState(FsmContext&);
    void onEnter(const DodgeDiveEvent&);
    void onEnter(DisarmedFallingState&);
    void onLeave(const DodgeEndEvent&);

private:
    void setAnim();

private:
    std::optional<float> jumpAttemptTimer;
    std::optional<float> dodgeAnimSubId;
    std::string animName;
};
}  // namespace Components
}  // namespace GameEngine
