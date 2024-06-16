#include "ArmedAttackState.h"

namespace GameEngine
{
namespace Components
{
ArmedAttackState::ArmedAttackState(FsmContext &context)
    : AttackStateBase{context, context.animClipNames.armed.attack}
{
}
}  // namespace Components
}  // namespace GameEngine
