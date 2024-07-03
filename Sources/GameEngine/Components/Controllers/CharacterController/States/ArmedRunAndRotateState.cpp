#include "ArmedRunAndRotateState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
ArmedRunAndRotateState::ArmedRunAndRotateState(FsmContext &context)
    : RotatingMoveAndRotateStateBase{context,
                                     std::nullopt,
                                     context.runSpeed,
                                     context.animClipNames.armed.run,
                                     context.animClipNames.armed.rotateLeft,
                                     context.animClipNames.armed.rotateRight}
{
}
}  // namespace Components
}  // namespace GameEngine
