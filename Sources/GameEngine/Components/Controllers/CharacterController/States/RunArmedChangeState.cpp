#include "RunArmedChangeState.h"

#include "../CharacterController.h"

namespace GameEngine
{
namespace Components
{
RunArmedChangeState::RunArmedChangeState(FsmContext& context)
    : ArmedChangeStateBase(context, context.upperBodyGroupName)
    , MoveStateBase{context, context.lowerBodyGroupName, context.runSpeed, context.animClipNames.disarmed.run.forward,
                    context.animClipNames.disarmed.run.backward}
    , context_{context}
{
}
void RunArmedChangeState::onEnter(const SprintStartEvent& event)
{
    MoveStateBase::onEnter(event);
    ArmedChangeStateBase::update(event);
}
void RunArmedChangeState::onEnter(const SprintStateChangeEvent& event)
{
    MoveStateBase::onEnter(event);
    ArmedChangeStateBase::update(event);
}
void RunArmedChangeState::onEnter(DisarmedRunState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::equipWeapon();
}
void RunArmedChangeState::onEnter(ArmedRunState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
}
void RunArmedChangeState::onEnter(DisarmedRunState&, const DrawArrowEvent&)
{
    ArmedChangeStateBase::equipWeapon();
}

void RunArmedChangeState::update(float dt)
{
    MoveStateBase::update(dt);
}

void RunArmedChangeState::update(const SprintStateChangeEvent& event)
{
    ArmedChangeStateBase::update(event);
}
}  // namespace Components
}  // namespace GameEngine
