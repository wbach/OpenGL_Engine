#pragma once
#include "Common.h"

namespace GameEngine
{
namespace Components
{
class StateMachine;

struct Context
{
    Pose& currentPose;
    StateMachine& machine;
    const JointGroupsIds& jointGroups;
    float transitionTime{0.125f};
};
}  // namespace Components
}  // namespace GameEngine
