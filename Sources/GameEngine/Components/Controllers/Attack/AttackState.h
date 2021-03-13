#pragma once
#include <Utils/Fsm/Actions.h>

#include "AttackEvents.h"
#include "AttackFsmContext.h"

namespace GameEngine
{
namespace Components
{
class EmptyState;

class AttackState : public Utils::StateMachine::Will<
                        Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                        Utils::StateMachine::On<AttackFsmEvents::End, Utils::StateMachine::TransitionTo<EmptyState>>>
{
public:
    AttackState(AttackFsmContext&);
    void onEnter(const AttackFsmEvents::Attack&);
    void onEnter(const AttackFsmEvents::AttackGrouped&);

private:
    AttackFsmContext& context_;
};
}  // namespace Components
}  // namespace GameEngine
