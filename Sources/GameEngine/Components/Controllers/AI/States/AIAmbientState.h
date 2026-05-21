#pragma once
#include <Utils/Fsm/Actions.h>

namespace GameEngine
{
class AIAmbientState : public Utils::StateMachine::Will<Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>>
{
public:
    AIAmbientState(){}
    //void onEnter();

private:
    // DialogContext& dialogContext;
};
}  // namespace GameEngine
