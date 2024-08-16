#include "ArmedSprintState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
ArmedSprintState::ArmedSprintState(FsmContext &context)
    : RotatingMoveState{context, std::nullopt, context.sprintSpeed, context.animClipNames.armed.sprint}
{
}

void ArmedSprintState::onMoveInactivity()
{
    setAnim(context_.animClipNames.armed.posture.stand.idle);
}
}  // namespace Components
}  // namespace GameEngine
