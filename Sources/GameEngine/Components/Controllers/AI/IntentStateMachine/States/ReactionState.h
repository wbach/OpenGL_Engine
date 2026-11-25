#pragma once
#include <Utils/Fsm/Actions.h>

#include "../../ReactionStateMachine/ReactionStateContext.h"
#include "../../ReactionStateMachine/ReactionStateMachine.h"
#include "../../ReactionStateMachine/States/CombatState/CombatStateEvents.h"
#include "../IntentStateEvents.h"

namespace GameEngine
{
namespace Components
{
struct IntentContext;
class ReactionState
    : public Utils::StateMachine::Will<Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                                       Utils::StateMachine::On<ReactionAIEvent, Utils::StateMachine::Update>,
                                       Utils::StateMachine::On<CombatStateEvent, Utils::StateMachine::Update>,
                                       Utils::StateMachine::On<ThreatEnded, Utils::StateMachine::BackToPreviousState>>
{
public:
    ReactionState(IntentContext&);
    ReactionState(ReactionState&&) = default;
    ~ReactionState();

    void onEnter();
    void onEnter(const ReactionAIEvent&);
    void update(float);
    void update(const ReactionAIEvent&);
    void update(const CombatStateEvent&);
    void onLeave();

    template <typename... T>
    bool isCurrentStateOfType() const
    {
        return reactionStateMachine_->isCurrentStateOfType<T...>();
    }

private:
    void handleEvent(const ReactionAIEvent&);

private:
    std::unique_ptr<ReactionStateContext> reactionStateContext_;
    std::unique_ptr<ReactionStateMachine> reactionStateMachine_;
};
}  // namespace Components
}  // namespace GameEngine
