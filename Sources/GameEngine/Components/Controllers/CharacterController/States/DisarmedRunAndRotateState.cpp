#include "DisarmedRunAndRotateState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
DisarmedRunAndRotateState::DisarmedRunAndRotateState(FsmContext& context)
    : Disarmed(context.animClipNames)
    , RotatingMoveAndRotateStateBase{context, std::nullopt, context.runSpeed, context.animClipNames.disarmed.movement.run,
                                     context.animClipNames.disarmed.posture.stand.rotate}
{
}
}  // namespace Components
}  // namespace GameEngine
