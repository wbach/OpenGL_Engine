#pragma once
#include <Utils/Fsm/Actions.h>

namespace GameEngine
{
namespace Components
{
class AIAttackState : public Utils::StateMachine::Will<Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>>
{
public:
    AIAttackState()
    {
    }
    void update(float)
    {
    }

private:
    // DialogContext& dialogContext;
};
}  // namespace Components
}  // namespace GameEngine
