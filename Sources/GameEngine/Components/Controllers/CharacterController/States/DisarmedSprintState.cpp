#include "DisarmedSprintState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
DisarmedSprintState::DisarmedSprintState(FsmContext &context)
    : RotatingMoveState{context, std::nullopt, context.sprintSpeed, context.animClipNames.disarmed.sprint}
{
}
void DisarmedSprintState::onMoveInactivity()
{
    setAnim(context_.animClipNames.disarmed.posture.stand.idle);
}
}  // namespace Components
}  // namespace GameEngine
