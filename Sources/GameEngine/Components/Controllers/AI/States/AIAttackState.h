#pragma once
#include <Utils/Fsm/Actions.h>

#include <optional>

#include "Fsm/actions/Update.h"
#include "Types.h"

namespace GameEngine
{
class GameObject;
struct EndAttackEvent;

namespace Components
{
struct AIControllerContext;
struct TargetOutOfAttackRangeEvent;
struct TargetLostEvent;
struct TargetInAttackRangeEvent;
class AIChaseState;
class AIReturnState;

class AIAttackState : public Utils::StateMachine::Will<
                          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                          Utils::StateMachine::On<TargetLostEvent, Utils::StateMachine::TransitionTo<AIReturnState>>,
                          Utils::StateMachine::On<TargetOutOfAttackRangeEvent, Utils::StateMachine::TransitionTo<AIChaseState>>>
{
public:
    AIAttackState(AIControllerContext&);
    ~AIAttackState();

    void onEnter();
    void onEnter(const TargetInAttackRangeEvent&);
    void onLeave();
    void update(float);
    void process(const EndAttackEvent&);

private:
    AIControllerContext& context;
    IdType characterControllerEventSubId;
    GameObject* target{nullptr};
};
}  // namespace Components
}  // namespace GameEngine
