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
    float transitionTime{1.f};
};
}  // namespace Components
}  // namespace GameEngine
