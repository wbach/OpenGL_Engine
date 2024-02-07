#include "WalkArmedChangeState.h"

namespace GameEngine
{
namespace Components
{
WalkArmedChangeState::WalkArmedChangeState(FsmContext &context)
    : ArmedChangeStateBase(context, context.upperBodyGroupName)
    , MoveStateBase{context, context.lowerBodyGroupName, context.walkSpeed, context.animClipNames.disarmed.walk.forward,
                    context.animClipNames.disarmed.walk.backward}
{
}
void WalkArmedChangeState::onEnter(const SprintStartEvent& event)
{
    MoveStateBase::onEnter(event);
    ArmedChangeStateBase::update(event);
}
void WalkArmedChangeState::onEnter(DisarmedWalkState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::equipWeapon();
}
void WalkArmedChangeState::onEnter(ArmedWalkState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
}
void WalkArmedChangeState::onEnter(DisarmedWalkState&, const DrawArrowEvent&)
{
    ArmedChangeStateBase::equipWeapon();
}
void WalkArmedChangeState::onEnter(DisarmedIdleState&, const DrawArrowEvent&)
{
    ArmedChangeStateBase::equipWeapon();
}

void WalkArmedChangeState::onEnter(DisarmedIdleState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::equipWeapon();
}

void WalkArmedChangeState::onEnter(ArmedIdleState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
}
void WalkArmedChangeState::update(float dt)
{
    MoveStateBase::update(dt);
}
void WalkArmedChangeState::update(const SprintStateChangeEvent& event)
{
    ArmedChangeStateBase::update(event);
}
}  // namespace Components
}  // namespace GameEngine
