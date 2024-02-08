#include "DisarmedRunState.h"

namespace GameEngine
{
namespace Components
{
DisarmedRunState::DisarmedRunState(FsmContext &context)
    : MoveStateBase{context, std::nullopt, context.runSpeed, context.animClipNames.disarmed.run.forward,
                    context.animClipNames.disarmed.run.backward}
{
}
}  // namespace Components
}  // namespace GameEngine
