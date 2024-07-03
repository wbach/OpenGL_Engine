#include "DisarmedAttackState.h"
#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
DisarmedAttackState::DisarmedAttackState(FsmContext &context)
    : AttackStateBase{context, context.animClipNames.disarmed.attack}
{
}
}  // namespace Components
}  // namespace GameEngine
