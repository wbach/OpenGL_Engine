#pragma once
#include <Types.h>

#include <variant>

namespace GameEngine
{
namespace Components
{
struct QuestTriggered
{
};
struct QuestEnded
{
};
struct ThreatEnded
{
};
struct DeathEvent
{
};
using IntentAIEvent = std::variant<QuestTriggered, QuestEnded, ThreatEnded, DeathEvent>;

}  // namespace Components
}  // namespace GameEngine
