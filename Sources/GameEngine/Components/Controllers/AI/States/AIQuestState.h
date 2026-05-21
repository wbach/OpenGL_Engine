#pragma once
#include <Utils/Fsm/Actions.h>

#include "../AIControllerEvents.h"

namespace GameEngine
{
namespace Components
{
class AIAmbientState;
struct AIControllerContext;

class AIQuestState : public Utils::StateMachine::Will<
                         Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                         Utils::StateMachine::On<QuestFinishedEvent, Utils::StateMachine::TransitionTo<AIAmbientState>>>
{
public:
    AIQuestState(AIControllerContext&);
    void onEnter(const QuestTriggeredEvent&);
    void update(float);
    void onLeave();

private:
    void updateNavigation();

private:
    AIControllerContext& context_;
    AIMoveType moveType_;
    bool isMovingForward_{false};

    // DialogContext& dialogContext;
};
}  // namespace Components
}  // namespace GameEngine
