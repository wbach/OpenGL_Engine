#include "ArmedIdleCrouchState.h"

#include "../FsmContext.h"
#include "GameEngine/Components/Controllers/CharacterController/States/BaseState.h"

namespace GameEngine
{
namespace Components
{
ArmedIdleCrouchState::ArmedIdleCrouchState(FsmContext &context)
    : BaseState(context)
    , IdleStateBase(context, context.animClipNames.armed.posture.crouched.idle)
{
}
}  // namespace Components
}  // namespace GameEngine
