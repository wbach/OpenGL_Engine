#pragma once
#include <Utils/Fsm/Actions.h>

namespace GameEngine
{
class AIAttackState : public Utils::StateMachine::Will<Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>>
{
public:
    AIAttackState(){}
    //void onEnter();

private:
    // DialogContext& dialogContext;
};
}  // namespace GameEngine
