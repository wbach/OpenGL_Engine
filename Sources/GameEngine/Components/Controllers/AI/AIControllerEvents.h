#pragma once
#include <Types.h>

#include <variant>

namespace GameEngine
{
class GameObject;
struct AIUpdateEvent
{
    float deltaTime = 0.0f;
};

struct TargetSpottedEvent
{
    GameObject* target = nullptr;
};

struct TargetLostEvent
{
};

struct TargetInAttackRangeEvent
{
};

struct TargetOutofAttackRangeEvent
{
};

struct QuestTriggeredEvent
{
    int questId = 0;
    vec3 targetPosition{0.0f};
};

struct QuestFinishedEvent
{
};

struct CurrentTargetDiedEvent
{
};

struct NavigationPathCompletedEvent
{
};

using AIEvent =
    std::variant<AIUpdateEvent, TargetSpottedEvent, TargetLostEvent, TargetInAttackRangeEvent, TargetOutofAttackRangeEvent,
                 QuestTriggeredEvent, QuestFinishedEvent, CurrentTargetDiedEvent, NavigationPathCompletedEvent>;
}  // namespace GameEngine
