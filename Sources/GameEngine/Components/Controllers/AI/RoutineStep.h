#pragma once
#include "AIBehaviorState.h"
#include <Types.h>

namespace GameEngine
{
namespace Components
{
struct RoutineStep
{
    int startHour   = 0;
    int startMinute = 0;
    AIBehaviorState behaviorState;
    vec3 targetPosition;

    int GetMinutesFromStartOfDay() const;
};
}  // namespace Components
}  // namespace GameEngine
