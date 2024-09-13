#include "DisarmedFallingState.h"

#include "../FsmContext.h"
#include "CharacterControllerCommonDefs.h"

namespace GameEngine
{
namespace Components
{
DisarmedFallingState::DisarmedFallingState(FsmContext &context)
    : FallingStateBase{context, context.animClipNames.disarmed.falling}
{
}
}  // namespace Components
}  // namespace GameEngine
