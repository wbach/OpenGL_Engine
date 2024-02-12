#pragma once
#include <Utils/Fsm/Fsm.h>
#include "States/AimState.h"
#include "States/RotateableRunState.h"
#include "States/TransitionState.h"
#include "States/FollowingState.h"

namespace GameEngine
{
namespace Components
{
namespace Camera
{
using ThridPersonCameraFsm = Utils::StateMachine::Fsm<FollowingState, RotateableRunState, AimState, TransitionState>;
}
}  // namespace Components
}  // namespace GameEngine
