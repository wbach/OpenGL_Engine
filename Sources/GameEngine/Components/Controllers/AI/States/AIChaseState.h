#pragma once
#include <Utils/Fsm/Actions.h>

namespace GameEngine
{
class AIChaseState : public Utils::StateMachine::Will<Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>>
{
public:
    AIChaseState(){}
    //void onEnter();

private:
    // DialogContext& dialogContext;
};
}  // namespace GameEngine
