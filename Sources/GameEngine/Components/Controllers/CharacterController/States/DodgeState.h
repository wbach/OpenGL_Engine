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
                       Utils::StateMachine::On<StartFallingEvent, Utils::StateMachine::TransitionTo<FallingState>>, // Suspicious / Copy-Paste Anomaly: DodgeState.h transitions to FallingState regardless of whether the prior state was armed or disarmed.
                       Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
                       Utils::StateMachine::On<DodgeEndEvent, Utils::StateMachine::BackToPreviousState>>
{
public:
    DodgeState(FsmContext&);
    ~DodgeState() override;

    void onEnter(const DodgeDiveEvent&);
    void onEnter(const DodgeForwardEvent&);
    void onEnter(const DodgeBackwardEvent&);
    void onEnter(const DodgeLeftEvent&);
    void onEnter(const DodgeRightEvent&);

    void onEnter(FallingState&);
    void onLeave();
    void onLeave(const DodgeEndEvent&);

    void update(float);

private:
    void onEnterDodge(const std::string& animName, const vec3& direction);
    void setAnimAndSubscribeForEnd(const std::string&);
    void setAnim(const std::string&);
    void unsubscribeAnimation();

private:
    std::optional<IdType> dodgeAnimSubId;
    vec3 dodgeDirection_{VECTOR_FORWARD};
    bool useRootMontion{false};
};
}  // namespace Components
}  // namespace GameEngine
