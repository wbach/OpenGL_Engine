#include "DisarmedRotateState.h"

#include "../FsmContext.h"
#include "RotateStateBase.h"

namespace GameEngine
{
namespace Components
{
DisarmedRotateState::DisarmedRotateState(FsmContext &context)
    : RotateStateBase{context, std::nullopt, context.runSpeed.rotate, context.animClipNames.disarmed.posture.stand.rotate,
                      RotateStateBase::CameraRotationPolicy::none}
{
}
}  // namespace Components
}  // namespace GameEngine
