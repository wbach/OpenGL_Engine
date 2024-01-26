#include "AimWalkState.h"

#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
AimWalkState::AimWalkState(FsmContext& context)
    : AimStateBase(context)
    , MoveStateBase{context, context.walkSpeed, context.animClipNames.armed.walk.forward,
                    context.animClipNames.armed.walk.backward}
{
}
void AimWalkState::update(float)
{
}
}  // namespace Components
}  // namespace GameEngine
