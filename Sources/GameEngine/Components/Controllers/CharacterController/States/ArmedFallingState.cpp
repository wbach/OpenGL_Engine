#include "ArmedFallingState.h"

#include "../FsmContext.h"
#include "CharacterControllerCommonDefs.h"

namespace GameEngine
{
namespace Components
{
ArmedFallingState::ArmedFallingState(FsmContext &context)
    : FallingStateBase{context, context.animClipNames.armed.falling}
{
}
}  // namespace Components
}  // namespace GameEngine
