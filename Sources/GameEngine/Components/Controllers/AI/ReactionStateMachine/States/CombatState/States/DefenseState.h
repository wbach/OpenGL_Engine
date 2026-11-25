#pragma once
#include <Utils/Fsm/Actions.h>

namespace GameEngine
{
namespace Components
{
class DefenseState : public Utils::StateMachine::Will<Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>>
{
public:
    DefenseState()
    {
    }

    void onEnter(){}
    void update(float){}
};
}  // namespace Components
}  // namespace GameEngine
