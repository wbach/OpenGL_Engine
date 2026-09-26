#pragma once
#include <Types.h>

#include <variant>

namespace GameEngine
{
namespace Components
{
struct DamageTaken
{
    int damageAmount{0};
    uint64_t attackerId{0};  // Kto nas zaatakował
    // opcjonalnie: Vector3 hitDirection;
};

struct TargetSpotted
{
    uint64_t targetId{0};
    bool isImmediateThreat{true};  // Czy od razu atakuje, czy tylko grozi
};

struct NoiseHeard
{
    // Vector3 noisePosition;
    float loudness{1.0f};
};

struct TargetLost
{
    uint64_t targetId{0};
};


struct LowHealthReached
{
    float currentHealthPercent{0.0f};
};

struct MoraleBroken
{
    float remainingMorale{0.0f};
};

struct PathBlocked
{
};

struct ThreatCleared
{
};


struct DialogueStarted
{
    uint64_t interactorId{0};
};

struct DialogueEnded
{
};

struct QuestTriggered
{
    uint32_t questId{0};
    uint32_t stepId{0};
};

struct ScheduleUpdated
{
    uint8_t newHour{0};
};

using MindAIEventVariant =
    std::variant<DamageTaken, TargetSpotted, NoiseHeard, TargetLost, LowHealthReached, MoraleBroken, PathBlocked, ThreatCleared,
                 DialogueStarted, DialogueEnded, QuestTriggered, ScheduleUpdated>;

}  // namespace Components
}  // namespace GameEngine
