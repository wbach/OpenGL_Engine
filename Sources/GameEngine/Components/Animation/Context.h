#pragma once
#include "Common.h"

namespace GameEngine
{
namespace Components
{
struct StateMachine;

const float DEFAULT_ANIMATION_TRANSITION_TIME{0.125f};

struct Context
{
    Pose& currentPose;
    StateMachine& machine;
    const JointGroupsIds& jointGroups;
    float transitionTime{DEFAULT_ANIMATION_TRANSITION_TIME};
};
}  // namespace Components
}  // namespace GameEngine
