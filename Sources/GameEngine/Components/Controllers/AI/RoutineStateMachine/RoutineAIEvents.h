#pragma once
#include <Types.h>

#include <variant>

namespace GameEngine
{
namespace Components
{

struct ShiftStarted
{
    glm::vec3 workLocation;
};

struct ShiftEnded
{
};

struct HungerThresholdReached
{
    glm::vec3 preferredFoodSource;
};

struct MealFinished
{
};

struct FatigueThresholdReached
{
};

struct WakeUp
{
};

struct UnderAttack
{
    unsigned int attackerId;
};

struct WeatherChanged
{
    bool isRaining;
};

using RoutineAIEvent = std::variant<ShiftStarted, ShiftEnded, HungerThresholdReached, MealFinished, FatigueThresholdReached,
                                    WakeUp, UnderAttack, WeatherChanged>;

}  // namespace Components
}  // namespace GameEngine
