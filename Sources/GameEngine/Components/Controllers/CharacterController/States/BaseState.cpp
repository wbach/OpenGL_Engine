#include "BaseState.h"

#include "../FsmContext.h"

namespace GameEngine::Components
{

BaseState::BaseState(FsmContext &context)
    : context_{context}
{
}

void BaseState::update(float)
{
}
}  // namespace GameEngine::Components
