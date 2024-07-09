#include "DisarmedAttackAndRotateState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
DisarmedAttackAndRotateState::DisarmedAttackAndRotateState(FsmContext &context)
    : AttackStateBase{context, context.animClipNames.disarmed.attack, context.upperBodyGroupName}
    , RotateStateBase{context, context.lowerBodyGroupName, context.runSpeed.rotate,
                      context.animClipNames.disarmed.rotateLeft, context.animClipNames.disarmed.rotateRight}
{
}

void DisarmedAttackAndRotateState::onEnter(const EndForwardMoveEvent &e)
{
    RotateStateBase::onEnter(e);
    subscribe();
}

void DisarmedAttackAndRotateState::onEnter(const EndMoveRightEvent &e)
{
    RotateStateBase::onEnter(e);
    subscribe();
}

void DisarmedAttackAndRotateState::onEnter(const EndMoveLeftEvent &e)
{
    RotateStateBase::onEnter(e);
    subscribe();
}

void DisarmedAttackAndRotateState::onEnter(const EndBackwardMoveEvent &e)
{
    RotateStateBase::onEnter(e);
    subscribe();
}
void DisarmedAttackAndRotateState::update(float v)
{
    AttackStateBase::update(v);
    RotateStateBase::update(v);
}
}  // namespace Components
}  // namespace GameEngine
