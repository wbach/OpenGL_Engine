#include "RunAndRotateArmedChangeState.h"
#include "../CharacterController.h"

namespace GameEngine
{
namespace Components
{
RunAndRotateArmedChangeState::RunAndRotateArmedChangeState(FsmContext &context)
    : ArmedChangeStateBase(context, context.upperBodyGroupName)
    , MoveAndRotateStateBase{context,
                             context.lowerBodyGroupName,
                             context.runSpeed,
                             context.animClipNames.disarmed.run,
                             context.animClipNames.disarmed.rotateLeft,
                             context.animClipNames.disarmed.rotateRight}
    , context_{context}
{
}
void RunAndRotateArmedChangeState::onEnter(const SprintStartEvent& event)
{
    MoveStateBase::onEnter(event);
    ArmedChangeStateBase::update(event);
}
void RunAndRotateArmedChangeState::onEnter(DisarmedRunAndRotateState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::equipWeapon();
}
void RunAndRotateArmedChangeState::onEnter(DisarmedRunAndRotateState&, const DrawArrowEvent&)
{
    ArmedChangeStateBase::equipWeapon();
}
void RunAndRotateArmedChangeState::onEnter(ArmedRunAndRotateState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
}
void RunAndRotateArmedChangeState::update(float dt)
{
    MoveAndRotateStateBase::update(dt);
}
}  // namespace Components
}  // namespace GameEngine
