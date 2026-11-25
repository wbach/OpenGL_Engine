#pragma once
#include <Types.h>

#include <variant>

namespace GameEngine
{
class GameObject;
namespace Components
{
struct DamageTaken
{
    GameObject* target{nullptr};
    int damageAmount{0};
};

struct TargetSpotted
{
    GameObject* target{nullptr};
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

struct ScheduleUpdated
{
    uint8_t newHour{0};
};

using ReactionAIEvent = std::variant<DamageTaken, TargetSpotted, NoiseHeard, TargetLost, LowHealthReached, MoraleBroken,
                                     PathBlocked, ThreatCleared, ScheduleUpdated>;

}  // namespace Components
}  // namespace GameEngine
