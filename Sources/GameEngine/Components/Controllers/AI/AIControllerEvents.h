#pragma once
#include "ActionStateMachine/ActionAIEvents.h"

namespace GameEngine
{
namespace Components
{
//using AIEvent = std::variant<MindAIEvent, ActionAIEvent>;
using AIEvent = ActionAIEvent;
}
}  // namespace GameEngine
