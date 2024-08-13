#include "DisarmedRotateState.h"

#include "../FsmContext.h"
#include "RotateStateBase.h"

namespace GameEngine
{
namespace Components
{
DisarmedRotateState::DisarmedRotateState(FsmContext &context)
    : RotateStateBase{context,
                      std::nullopt,
                      context.runSpeed.rotate,
                      context.animClipNames.disarmed.rotateLeft,
                      context.animClipNames.disarmed.rotateRight,
                      RotateStateBase::CameraRotationPolicy::none}
{
}
}  // namespace Components
}  // namespace GameEngine
