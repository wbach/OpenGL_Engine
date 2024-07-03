#include "MoveJumpState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
MoveJumpState::MoveJumpState(FsmContext &context, std::function<void()> endCallback)
    : JumpStateBase(context, std::nullopt, endCallback)
{
}
}  // namespace Components
}  // namespace GameEngine
