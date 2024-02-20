#include "DisarmedRunState.h"

namespace GameEngine
{
namespace Components
{
DisarmedRunState::DisarmedRunState(FsmContext &context)
    : MoveStateBase{context, std::nullopt, context.runSpeed, context.animClipNames.disarmed.run, context.animClipNames.disarmed.idle}
{
}
}  // namespace Components
}  // namespace GameEngine
