#pragma once
#include <Utils/Fsm/Actions.h>

namespace GameEngine
{
namespace Components
{
class AIChaseState : public Utils::StateMachine::Will<Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>>
{
public:
    AIChaseState()
    {
    }
    // void onEnter();
    void update(float)
    {
    }

private:
    // DialogContext& dialogContext;
};
}  // namespace Components
}  // namespace GameEngine
