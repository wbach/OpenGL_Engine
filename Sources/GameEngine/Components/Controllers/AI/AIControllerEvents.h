#pragma once
#include "ActionStateMachine/ActionAIEvents.h"
#include "IntentStateMachine/IntentStateEvents.h"
#include "ReactionStateMachine/ReactionAIEvents.h"
#include "ReactionStateMachine/States/CombatState/CombatStateEvents.h"

namespace GameEngine
{
namespace Components
{
using AIEvent = std::variant<ActionAIEvent, IntentAIEvent, ReactionAIEvent, CombatStateEvent>;
}
}  // namespace GameEngine
