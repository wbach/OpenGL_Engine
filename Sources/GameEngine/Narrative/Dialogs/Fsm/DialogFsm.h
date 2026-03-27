#pragma once
#include <Utils/Fsm/Fsm.h>

#include "IdleState.h"
#include "ShowingSentence.h"
#include "TransitionState.h"
#include "WaitingForInput.h"

namespace GameEngine
{
using DialogFsm = Utils::StateMachine::Fsm<IdleState, TransitionState, ShowingSentence, WaitingForInput>;
}  // namespace GameEngine
