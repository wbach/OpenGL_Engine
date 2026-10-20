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
struct EntityDiedEvent
{
};
using IntentAIEvent = std::variant<QuestTriggered, QuestEnded, ThreatEnded, EntityDiedEvent>;

}  // namespace Components
}  // namespace GameEngine
