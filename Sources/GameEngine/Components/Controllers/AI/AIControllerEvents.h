#pragma once
#include "ActionStateMachine/ActionAIEvents.h"
#include "IntentStateMachine/IntentStateEvents.h"
#include "ReactionStateMachine/ReactionAIEvents.h"

namespace GameEngine
{
namespace Components
{
using AIEvent = std::variant<ActionAIEvent, IntentAIEvent>;
}
}  // namespace GameEngine
