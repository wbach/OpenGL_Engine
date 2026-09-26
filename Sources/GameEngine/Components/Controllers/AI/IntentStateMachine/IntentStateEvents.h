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
struct DeathEvent
{
};
using IntentAIEvent = std::variant<QuestTriggered, QuestEnded, DeathEvent>;

}  // namespace Components
}  // namespace GameEngine
