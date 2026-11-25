#include "ReactionState.h"

#include <Utils/Utils.h>

#include "../IntentContext.h"

namespace GameEngine
{
namespace Components
{
ReactionState::ReactionState(IntentContext& intentContext)
    : reactionStateContext_{new ReactionStateContext{.gameObject          = intentContext.gameObject,
                                                     .controller          = intentContext.controller,
                                                     .navigationManager   = intentContext.navigationManager,
                                                     .characterController = intentContext.characterController,
                                                     .currentPath         = intentContext.currentPath}}
{
    reactionStateMachine_ = std::make_unique<ReactionStateMachine>(
        PassiveState{}, SuspiciousState{}, CombatState{*reactionStateContext_}, SurrenderedState{}, PanicState{}, RetreatState{});
}
ReactionState::~ReactionState()
{
}
void ReactionState::onEnter()
{
}
void ReactionState::onEnter(const ReactionAIEvent& reactionEvent)
{
    handleEvent(reactionEvent);
}
void ReactionState::update(float)
{
}
void ReactionState::onLeave()
{
}
void ReactionState::update(const ReactionAIEvent& reactionEvent)
{
    handleEvent(reactionEvent);
}
void ReactionState::update(const CombatStateEvent& combatStateEvent)
{
    reactionStateMachine_->handle(combatStateEvent);
}
void ReactionState::handleEvent(const ReactionAIEvent& reactionEvent)
{
    auto passEventToMachine = [&](const auto& event)
    {
        LOG_DEBUG << Utils::GetTypeName(event);
        reactionStateMachine_->handle(event);
    };
    std::visit(passEventToMachine, reactionEvent);
}
}  // namespace Components
}  // namespace GameEngine
