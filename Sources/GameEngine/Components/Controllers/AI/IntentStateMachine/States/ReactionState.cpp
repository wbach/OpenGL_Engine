#include "ReactionState.h"

#include <Utils/Utils.h>

namespace GameEngine
{
namespace Components
{
ReactionState::ReactionState()
{
    reactionStateMachine_ = std::make_unique<ReactionStateMachine>();
}
void ReactionState::onEnter()
{
}
void ReactionState::onEnter(const ReactionAIEvent& event)
{
    auto passEventToMachine = [&](const auto& e)
    {
        LOG_DEBUG << Utils::GetTypeName(e);
        reactionStateMachine_->handle(e);
    };
    std::visit(passEventToMachine, event);
}
void ReactionState::update(float)
{
}
void ReactionState::onLeave()
{
}
}  // namespace Components
}  // namespace GameEngine
