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
};

struct TargetOutofAttackRangeEvent
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
    std::variant<AIUpdateEvent, TargetSpottedEvent, TargetLostEvent, TargetInAttackRangeEvent, TargetOutofAttackRangeEvent,
                 QuestTriggeredEvent, QuestFinishedEvent, CurrentTargetDiedEvent, NavigationPathCompletedEvent>;
}  // namespace Components
}  // namespace GameEngine
