#pragma once
#include <Utils/Fsm/Actions.h>

#include "AIStateBase.h"

namespace GameEngine
{
namespace Components
{
class AIAmbientState;

class AIQuestState : public AIStateBase,
                     public Utils::StateMachine::Will<
                         Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                         Utils::StateMachine::On<QuestTriggeredEvent, Utils::StateMachine::Update>,
                         Utils::StateMachine::On<QuestFinishedEvent, Utils::StateMachine::TransitionTo<AIAmbientState>>>
{
public:
    AIQuestState(AIControllerContext&);

    using AIStateBase::onEnter;
    using AIStateBase::onLeave;

    void onEnter(const QuestTriggeredEvent&);
    void update(const QuestTriggeredEvent&);
    void update(float);
};
}  // namespace Components
}  // namespace GameEngine
