#include "DisarmedRunAndRotateState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
DisarmedRunAndRotateState::DisarmedRunAndRotateState(FsmContext& context)
    : DisarmedStateBase(context.animClipNames)
    , RotatingMoveAndRotateStateBase{context, std::nullopt, context.runSpeed, clips.movement.run, clips.posture.stand.rotate}
{
}
}  // namespace Components
}  // namespace GameEngine
