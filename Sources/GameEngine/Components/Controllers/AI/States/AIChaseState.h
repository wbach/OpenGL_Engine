#pragma once
#include <Utils/Fsm/Actions.h>

#include "../AIControllerEvents.h"
namespace GameEngine
{
namespace Components
{
struct AIAttackState;
struct AIControllerContext;
class AIChaseState : public Utils::StateMachine::Will<
                         Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                         Utils::StateMachine::On<TargetInAttackRangeEvent, Utils::StateMachine::TransitionTo<AIAttackState>>,
                         Utils::StateMachine::On<TargetLostEvent, Utils::StateMachine::BackToPreviousState>>

{
public:
    AIChaseState(AIControllerContext&);
    void onEnter(const TargetSpottedEvent&);
    void update(float);
    void onLeave();
    void moveTo(const vec3&);

private:
    AIControllerContext& context_;
    GameObject* target{nullptr};
    float pathUpdateTimer_{-1.f};
};
}  // namespace Components
}  // namespace GameEngine
