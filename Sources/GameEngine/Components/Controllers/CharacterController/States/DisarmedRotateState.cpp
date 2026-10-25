#include "DisarmedRotateState.h"

#include "../FsmContext.h"
#include "RotateStateBase.h"

namespace GameEngine
{
namespace Components
{
DisarmedRotateState::DisarmedRotateState(FsmContext& context)
    : DisarmedStateBase(context.animClipNames)
    , RotateStateBase{context, std::nullopt, context.runSpeed.rotate, clips.posture.stand.rotate,
                      RotateStateBase::CameraRotationPolicy::none}
{
}
}  // namespace Components
}  // namespace GameEngine
