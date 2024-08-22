#include "CharacterControllerCommonDefs.h"
#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
JumpState::JumpState(FsmContext &context)
    : JumpStateBase(context, std::nullopt)
{
}
}  // namespace Components
}  // namespace GameEngine
