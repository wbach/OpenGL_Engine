#pragma once
#include <Utils/Fsm/Actions.h>

namespace GameEngine
{
class AIQuestState : public Utils::StateMachine::Will<Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>>
{
public:
    AIQuestState(){}
    //void onEnter();

private:
    // DialogContext& dialogContext;
};
}  // namespace GameEngine
