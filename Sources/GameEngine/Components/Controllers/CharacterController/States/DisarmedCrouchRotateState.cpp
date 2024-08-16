#include "DisarmedCrouchRotateState.h"

#include "../FsmContext.h"
#include "RotateStateBase.h"

namespace GameEngine
{
namespace Components
{
DisarmedCrouchRotateState::DisarmedCrouchRotateState(FsmContext &context)
    : RotateStateBase{context, std::nullopt, context.runSpeed.rotate, context.animClipNames.disarmed.posture.crouched.rotate,
                      RotateStateBase::CameraRotationPolicy::none}
{
}
}  // namespace Components
}  // namespace GameEngine
