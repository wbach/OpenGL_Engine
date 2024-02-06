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
                             context.animClipNames.armed.walk,
                             context.animClipNames.armed.rotateLeft,
                             context.animClipNames.armed.rotateRight}
    , context_{context}
{
}

void WalkAndRotateArmedChangeState::update(float dt)
{
    MoveAndRotateStateBase::update(dt);
}

}  // namespace Components
}  // namespace GameEngine
