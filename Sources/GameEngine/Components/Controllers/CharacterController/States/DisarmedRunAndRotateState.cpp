#include "DisarmedRunAndRotateState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
DisarmedRunAndRotateState::DisarmedRunAndRotateState(FsmContext& context)
    : RotatingMoveAndRotateStateBase{context,
                                     std::nullopt,
                                     context.runSpeed,
                                     context.animClipNames.disarmed.run,
                                     context.animClipNames.disarmed.rotateLeft,
                                     context.animClipNames.disarmed.rotateRight}
{
}
}  // namespace Components
}  // namespace GameEngine
