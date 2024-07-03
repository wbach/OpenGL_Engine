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
                                     context.animClipNames.armed.rotateLeft,
                                     context.animClipNames.armed.rotateRight}
{
}
}  // namespace Components
}  // namespace GameEngine
