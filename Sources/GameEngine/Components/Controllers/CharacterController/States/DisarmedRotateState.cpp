#include "DisarmedRotateState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
DisarmedRotateState::DisarmedRotateState(FsmContext &context)
    : RotateStateBase{context, std::nullopt, context.runSpeed.rotate, context.animClipNames.disarmed.rotateLeft,
                      context.animClipNames.disarmed.rotateRight}
{
}
}  // namespace Components
}  // namespace GameEngine
