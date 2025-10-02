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
                       Utils::StateMachine::On<DodgeForwardEvent, Utils::StateMachine::Nothing>,
                       Utils::StateMachine::On<DodgeBackwardEvent, Utils::StateMachine::Nothing>,
                       Utils::StateMachine::On<DodgeLeftEvent, Utils::StateMachine::Nothing>,
                       Utils::StateMachine::On<DodgeRightEvent, Utils::StateMachine::Nothing>,
                       Utils::StateMachine::On<StartFallingEvent, Utils::StateMachine::TransitionTo<DisarmedFallingState>>,
                       Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
                       Utils::StateMachine::On<DodgeEndEvent, Utils::StateMachine::BackToPreviousState>>
{
public:
    DodgeState(FsmContext&);
    void onEnter(const DodgeDiveEvent&);
    void onEnter(const DodgeForwardEvent&);
    void onEnter(const DodgeBackwardEvent&);
    void onEnter(const DodgeLeftEvent&);
    void onEnter(const DodgeRightEvent&);

    void onEnter(DisarmedFallingState&);
    void onLeave(const DodgeEndEvent&);

private:
    void setAnimAndSubscribeForEnd(const std::string&);
    void setAnim(const std::string&);

private:
    std::optional<float> jumpAttemptTimer;
    std::optional<IdType> dodgeAnimSubId;
};
}  // namespace Components
}  // namespace GameEngine
