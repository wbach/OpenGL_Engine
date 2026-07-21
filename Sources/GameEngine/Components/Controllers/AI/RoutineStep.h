#pragma once
#include <Types.h>

#include "AIBehaviorState.h"

namespace GameEngine
{
namespace Components
{
struct RoutineStep
{
    int startHour   = 0;
    int startMinute = 0;
    AIBehaviorState behaviorState{AIBehaviorState::Idle};
    vec3 targetPosition{0.f};

    int GetMinutesFromStartOfDay() const;
};
}  // namespace Components
}  // namespace GameEngine
