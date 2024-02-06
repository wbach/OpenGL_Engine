#include "WalkAndRotateArmedChangeState.h"


namespace GameEngine
{
namespace Components
{
WalkAndRotateArmedChangeState::WalkAndRotateArmedChangeState(FsmContext& context)
    : ArmedChangeStateBase(context, context.upperBodyGroupName)
    , MoveAndRotateStateBase{context,
                             context.lowerBodyGroupName,
                             context.walkSpeed,
                             context.animClipNames.disarmed.walk,
                             context.animClipNames.disarmed.rotateLeft,
                             context.animClipNames.disarmed.rotateRight}
    , context_{context}
{
}

void WalkAndRotateArmedChangeState::update(float dt)
{
    MoveAndRotateStateBase::update(dt);
}

}  // namespace Components
}  // namespace GameEngine
