#include "ArmedRotateState.h"

#include "../CharacterController.h"

namespace GameEngine
{
namespace Components
{
ArmedRotateState::ArmedRotateState(FsmContext &context)
    : RotateStateBase{context, std::nullopt, context.runSpeed.leftRight, context.animClipNames.armed.rotateLeft,
                      context.animClipNames.armed.rotateRight}
{
}
}  // namespace Components
}  // namespace GameEngine
