#pragma once
#include <Utils/Fsm/Fsm.h>
#include "States/AimState.h"
#include "States/RotateableRunState.h"
#include "States/TransitionState.h"
#include "States/FollowingState.h"
#include "States/ScriptedState.h"

namespace GameEngine
{
namespace Components
{
namespace Camera
{
using ThridPersonCameraFsm = Utils::StateMachine::Fsm<FollowingState, RotateableRunState, AimState, TransitionState, ScriptedState>;
}
}  // namespace Components
}  // namespace GameEngine
