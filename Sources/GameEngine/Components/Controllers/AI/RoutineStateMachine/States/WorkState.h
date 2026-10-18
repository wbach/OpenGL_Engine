#pragma once
#include <Utils/Fsm/Actions.h>

#include "../RoutineAIEvents.h"

namespace GameEngine
{
namespace Components
{
class RestState;
class EatState;
class SleepState;
class SuspendState;
class ProfessionComponent;

struct RoutineContext;

class WorkState
    : public Utils::StateMachine::Will<Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                                       Utils::StateMachine::On<WorkStarted, Utils::StateMachine::Update>,
                                       Utils::StateMachine::On<MealStarted, Utils::StateMachine::TransitionTo<EatState>>,
                                       Utils::StateMachine::On<SleepStarted, Utils::StateMachine::TransitionTo<SleepState>>,
                                       Utils::StateMachine::On<Suspend, Utils::StateMachine::TransitionTo<SuspendState>>,
                                       Utils::StateMachine::On<Resume, Utils::StateMachine::TransitionTo<WorkState>>,
                                       Utils::StateMachine::On<RestStarted, Utils::StateMachine::TransitionTo<RestState>>>
{
public:
    WorkState(RoutineContext&);

    void onEnter();
    void onEnter(const WorkStarted&);
    void update(float);
    void update(const WorkStarted&);

private:
    RoutineContext& context;
    vec3 workLocation{0, 0, 0};
};
}  // namespace Components
}  // namespace GameEngine
