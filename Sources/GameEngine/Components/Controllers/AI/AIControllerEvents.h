#pragma once
#include "ActionStateMachine/ActionAIEvents.h"
#include "IntentStateMachine/IntentStateEvents.h"
#include "MindStateMachine/MindAIEvenets.h"
#include "RoutineStateMachine/RoutineAIEvents.h"
namespace GameEngine
{
namespace Components
{
using AIEvent = std::variant<MindAIEvent, ActionAIEvent, IntentAIEvent, RoutineAIEvent>;
}
}  // namespace GameEngine
