#include "ArmedSprintAndRotateState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
ArmedSprintAndRotateState::ArmedSprintAndRotateState(FsmContext &context)
    : RotatingMoveAndRotateStateBase{context,
                                     std::nullopt,
                                     context.sprintSpeed,
                                     context.animClipNames.armed.sprint,
                                     context.runSpeed.rotate,
                                     context.animClipNames.armed.posture.stand.rotate}
{
}
}  // namespace Components
}  // namespace GameEngine
