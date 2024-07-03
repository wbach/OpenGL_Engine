#include "ArmedRotateState.h"

#include "../CharacterController.h"
#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
ArmedRotateState::ArmedRotateState(FsmContext &context)
    : RotateStateBase{context, std::nullopt, context.runSpeed.rotate, context.animClipNames.armed.rotateLeft,
                      context.animClipNames.armed.rotateRight}
{
}
}  // namespace Components
}  // namespace GameEngine
