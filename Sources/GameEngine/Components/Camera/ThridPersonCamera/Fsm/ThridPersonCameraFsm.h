#pragma once
#include <Utils/Fsm/Fsm.h>
#include "States/AimState.h"
#include "States/RotateableRunState.h"

namespace GameEngine
{
namespace Components
{
namespace Camera
{
using ThridPersonCameraFsm = Utils::StateMachine::Fsm<RotateableRunState, AimState>;
}
}  // namespace Components
}  // namespace GameEngine
