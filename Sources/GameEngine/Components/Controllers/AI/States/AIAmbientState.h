#pragma once
#include <Utils/Fsm/Actions.h>

#include "../AIControllerEvents.h"

namespace GameEngine
{
namespace Components
{
class AIQuestState;
class AIChaseState;
class AIAmbientState : public Utils::StateMachine::Will<
                           Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                           Utils::StateMachine::On<TargetSpottedEvent, Utils::StateMachine::TransitionTo<AIChaseState>>,
                           Utils::StateMachine::On<QuestTriggeredEvent, Utils::StateMachine::TransitionTo<AIQuestState>>>
{
public:
    void update(float)
    {
    }
};
}  // namespace Components
}  // namespace GameEngine
