#include "DisarmedSprintAndRotateState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
DisarmedSprintAndRotateState::DisarmedSprintAndRotateState(FsmContext &context)
    : RotatingMoveAndRotateStateBase{context,
                                     std::nullopt,
                                     context.sprintSpeed,
                                     context.animClipNames.disarmed.sprint,
                                     context.runSpeed.rotate,
                                     context.animClipNames.disarmed.posture.stand.rotate}
{
}
}  // namespace Components
}  // namespace GameEngine
