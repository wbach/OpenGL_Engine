#pragma once
#include <Utils/Fsm/Actions.h>

#include "../AIControllerEvents.h"
namespace GameEngine
{
namespace Components
{
struct AIControllerContext;

class AIChaseState : public Utils::StateMachine::Will<Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>>
{
public:
    AIChaseState(AIControllerContext&);
    void onEnter(const TargetSpottedEvent&);
    void update(float);

private:
    AIControllerContext& context_;
};
}  // namespace Components
}  // namespace GameEngine
