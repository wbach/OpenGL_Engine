#pragma once
#include <Types.h>

#include <variant>

namespace GameEngine
{
namespace Components
{
struct WorkStarted
{
    vec3 workLocation;
};

struct MealStarted
{
    vec3 foodSource;
};

struct SleepStarted
{
};

struct RestStarted
{
};

struct Suspend
{
};

struct Resume
{
};

using RoutineAIEvent = std::variant<WorkStarted, MealStarted, SleepStarted, Suspend, Resume, RestStarted>;
}  // namespace Components
}  // namespace GameEngine
