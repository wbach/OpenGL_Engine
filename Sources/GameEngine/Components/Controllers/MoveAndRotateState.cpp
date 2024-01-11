#include "MoveAndRotateState.h"
#include "FsmContext.h"

namespace GameEngine
{
namespace Components
{
MoveAndRotateState::MoveAndRotateState(FsmContext &context)
    : MoveStateBase{context, context.animClipNames.disarmed.run.forward, context.animClipNames.disarmed.run.backward}
    , RotateStateBase{context, context.animClipNames.disarmed.run.rotateLeft,
                          context.animClipNames.disarmed.run.rotateRight }
{
}

void MoveAndRotateState::update(const AttackEvent& event)
{
    MoveStateBase::update(event);
}

void MoveAndRotateState::update(const EndAttackEvent& event)
{
    MoveStateBase::update(event);
}

void MoveAndRotateState::onEnter(const EndJumpEvent &event)
{
    MoveStateBase::onEnter(event);
}

void MoveAndRotateState::onEnter(const MoveForwardEvent &event)
{
    MoveStateBase::onEnter(event);
}

void MoveAndRotateState::onEnter(const MoveBackwardEvent &event)
{
    MoveStateBase::onEnter(event);
}

void MoveAndRotateState::onEnter(const RotateLeftEvent &event)
{
    RotateStateBase::onEnter(event);
}

void MoveAndRotateState::onEnter(const RotateRightEvent &event)
{
    RotateStateBase::onEnter(event);
}

void MoveAndRotateState::onEnter(const RotateTargetEvent &event)
{
    RotateStateBase::onEnter(event);
}

void MoveAndRotateState::update(float deltaTime)
{
    MoveStateBase::update(deltaTime);
    RotateStateBase::update(deltaTime);
}

}  // namespace Components
}  // namespace GameEngine
