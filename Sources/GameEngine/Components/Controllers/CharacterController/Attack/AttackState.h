#pragma once
#include <Utils/Fsm/Actions.h>

#include "AttackEvents.h"
#include "AttackFsmContext.h"

namespace GameEngine
{
namespace Components
{
struct EmptyState;

class AttackState : public Utils::StateMachine::Will<
                        Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                        Utils::StateMachine::On<AttackFsmEvents::Attack, Utils::StateMachine::Update>,
                        Utils::StateMachine::On<AttackFsmEvents::AttackGrouped, Utils::StateMachine::Update>,
                        Utils::StateMachine::On<AttackFsmEvents::End, Utils::StateMachine::TransitionTo<EmptyState>>>
{
public:
    AttackState(AttackFsmContext&);
    AttackState(const AttackState&) = delete;
    AttackState(AttackState&&);
    ~AttackState();
    void onEnter(const AttackFsmEvents::Attack&);
    void onEnter(const AttackFsmEvents::AttackGrouped&);
    void update(const AttackFsmEvents::Attack&);
    void update(const AttackFsmEvents::AttackGrouped&);
    void onLeave();

private:
    void subscribeForAnimationsEnd();

private:
    AttackFsmContext& context_;
    size_t animationIndex_;
    size_t currentAnimIndex_;
    std::vector<IdType> onAnimationEndSubIds;
};
}  // namespace Components
}  // namespace GameEngine
