#include "CombatState.h"

#include <GameEngine/Components/Controllers/AI/AIController.h>

#include "../ReactionStateContext.h"
#include <Logger/Log.h>

namespace GameEngine
{
namespace Components
{
CombatState::CombatState(ReactionStateContext& context)
    : reactionStateContext{context}
{
}
void CombatState::onEnter()
{
}
void CombatState::update(float)
{
}
void CombatState::onLeave(const TargetLost&)
{
    LOG_DEBUG << "";
    reactionStateContext.controller.pushEventToQueue(ThreatEnded{});
    LOG_DEBUG << "";
}
void CombatState::onLeave(const ThreatCleared&)
{
    LOG_DEBUG << "";
    reactionStateContext.controller.pushEventToQueue(ThreatEnded{});
    LOG_DEBUG << "";
}
}  // namespace Components
}  // namespace GameEngine
