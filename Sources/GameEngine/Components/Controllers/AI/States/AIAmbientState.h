#pragma once
#include <Utils/Fsm/Actions.h>

#include "../AIControllerEvents.h"

namespace GameEngine
{
namespace Components
{
class AIQuestState;
class AIAmbientState : public Utils::StateMachine::Will<
                           Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                           Utils::StateMachine::On<QuestTriggeredEvent, Utils::StateMachine::TransitionTo<AIQuestState>>>
{
public:
    AIAmbientState()
    {
    }

    void update(float)
    {
    }
    // void onEnter();

private:
    // DialogContext& dialogContext;
};
}  // namespace Components
}  // namespace GameEngine
