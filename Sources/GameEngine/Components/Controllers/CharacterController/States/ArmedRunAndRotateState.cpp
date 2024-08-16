#include "ArmedRunAndRotateState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
ArmedRunAndRotateState::ArmedRunAndRotateState(FsmContext &context)
    : RotatingMoveAndRotateStateBase{context, std::nullopt, context.runSpeed, context.animClipNames.armed.movement.run,
                                     context.animClipNames.armed.posture.stand.rotate}
{
}
}  // namespace Components
}  // namespace GameEngine
