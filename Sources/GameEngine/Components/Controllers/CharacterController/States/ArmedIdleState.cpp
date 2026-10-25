#include "ArmedIdleState.h"

#include "../FsmContext.h"
#include "GameEngine/Components/Controllers/CharacterController/States/BaseState.h"

namespace GameEngine
{
namespace Components
{
ArmedIdleState::ArmedIdleState(FsmContext &context)
    : BaseState(context)
    , ArmedStateBase(context.animClipNames)
    , IdleStateBase(context, context.animClipNames.armed.posture.stand.idle)
{
}
bool ArmedIdleState::leaveCondition(const AttackEvent &)
{
    return not clips.attack.empty();
}
bool ArmedIdleState::leaveCondition(const WeaponStateEvent &)
{
    return not context_.animClipNames.disarm.clipNames.empty();
}
}  // namespace Components
}  // namespace GameEngine
