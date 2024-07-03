#include "CharacterControllerCommonDefs.h"
#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
JumpState::JumpState(FsmContext &context, std::function<void()> endCallback)
    : JumpStateBase(context, std::nullopt, endCallback)
{
}
}  // namespace Components
}  // namespace GameEngine
