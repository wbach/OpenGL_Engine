#include "DisarmedSprintAndRotateState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
DisarmedSprintAndRotateState::DisarmedSprintAndRotateState(FsmContext& context)
    : DisarmedStateBase(context.animClipNames)
    , RotatingMoveAndRotateStateBase{
          context, std::nullopt, context.sprintSpeed, clips.sprint, context.runSpeed.rotate, clips.posture.stand.rotate}
{
}
}  // namespace Components
}  // namespace GameEngine
