#pragma once
#include <Utils/Fsm/Actions.h>

namespace GameEngine
{
namespace Components
{
class RestState : public Utils::StateMachine::Will<
                           Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>>
{
public:
    void onEnter();
    void update(float)
    {
    }
};
}  // namespace Components
}  // namespace GameEngine
