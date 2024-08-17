#include "ArmedCrouchRotateState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
ArmedCrouchRotateState::ArmedCrouchRotateState(FsmContext &context)
    : RotateStateBase{context, std::nullopt, context.runSpeed.rotate, context.animClipNames.armed.posture.crouched.rotate,
                      RotateStateBase::CameraRotationPolicy::none}
{
}
}  // namespace Components
}  // namespace GameEngine
