#pragma once
#include <Utils/Fsm/Fsm.h>
#include "AttackState.h"
#include "EmptyState.h"

namespace GameEngine
{
namespace Components
{
using AttackFsm = Utils::StateMachine::Fsm<EmptyState, AttackState>;
}
}  // namespace GameEngine
