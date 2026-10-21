#pragma once
#include <Utils/Fsm/Actions.h>

#include "../../ReactionStateMachine/ReactionStateMachine.h"
#include "../IntentStateEvents.h"

namespace GameEngine
{
namespace Components
{
class ReactionState
    : public Utils::StateMachine::Will<Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                                       Utils::StateMachine::On<ThreatEnded, Utils::StateMachine::BackToPreviousState>>
{
public:
    ReactionState();
    void onEnter();
    void onEnter(const ReactionAIEvent&);
    void update(float);
    void onLeave();

private:
    std::unique_ptr<ReactionStateMachine> reactionStateMachine_;
};
}  // namespace Components
}  // namespace GameEngine
