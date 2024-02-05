#include "IdleArmedChangeState.h"

namespace GameEngine
{
namespace Components
{
IdleArmedChangeState::IdleArmedChangeState(FsmContext &context)
    : IdleStateBase{context, context.animClipNames.disarmed.idle}
{
}
void IdleArmedChangeState::onEnter(const WeaponStateEvent &)
{

}
}  // namespace Components
}  // namespace GameEngine
