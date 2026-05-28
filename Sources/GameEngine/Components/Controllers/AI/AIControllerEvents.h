#pragma once
#include <Types.h>

#include <variant>

namespace GameEngine
{
class GameObject;
namespace Components
{
enum AIMoveType
{
    WALK,
    RUN,
    SPRINT
};

struct AIUpdateEvent
{
    float deltaTime = 0.0f;
};

struct TargetSpottedEvent
{
    GameObject& target;
};

struct TargetLostEvent
{
};

struct TargetInAttackRangeEvent
{
    GameObject& target;
};

struct TargetOutOfAttackRangeEvent
{
};

struct QuestTriggeredEvent
{
    vec3 targetPosition{0.0f};
    AIMoveType moveType{AIMoveType::RUN};
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
    std::variant<AIUpdateEvent, TargetSpottedEvent, TargetLostEvent, TargetInAttackRangeEvent, TargetOutOfAttackRangeEvent,
                 QuestTriggeredEvent, QuestFinishedEvent, CurrentTargetDiedEvent, NavigationPathCompletedEvent>;
}  // namespace Components
}  // namespace GameEngine
