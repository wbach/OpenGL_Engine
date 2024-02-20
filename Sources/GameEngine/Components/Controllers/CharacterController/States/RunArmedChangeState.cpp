#include "RunArmedChangeState.h"

#include "../CharacterController.h"

namespace GameEngine
{
namespace Components
{
RunArmedChangeState::RunArmedChangeState(FsmContext& context)
    : ArmedChangeStateBase(context, context.upperBodyGroupName)
    , MoveStateBase{context, context.lowerBodyGroupName, context.runSpeed, context.animClipNames.disarmed.run}
    , context_{context}
{
}

void RunArmedChangeState::onEnter()
{
    if (context_.weaponArmedChangeState == FsmContext::WeaponArmedChangeState::Equip)
    {
        MoveStateBase::changeAnimationClips(context_.animClipNames.armed.run);
    }
    else if (context_.weaponArmedChangeState == FsmContext::WeaponArmedChangeState::Disarm)
    {
        MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.run);
    }
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
    MoveStateBase::changeAnimationClips(context_.animClipNames.armed.run);
}
void RunArmedChangeState::onEnter(ArmedRunState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.run);

}
void RunArmedChangeState::onEnter(DisarmedRunState&, const DrawArrowEvent& e)
{
    ArmedChangeStateBase::equipWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.armed.run);
    ArmedChangeStateBase::update(e);
}

void RunArmedChangeState::onEnter(DisarmedSprintState &, const DrawArrowEvent & e)
{
    ArmedChangeStateBase::equipWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.armed.run);
    ArmedChangeStateBase::update(e);

}

void RunArmedChangeState::onEnter(DrawArrowWalkState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.run);

}

void RunArmedChangeState::onEnter(RecoilWalkState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.run);

}

void RunArmedChangeState::onEnter(AimWalkState&, const WeaponStateEvent&)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.run);

}

void RunArmedChangeState::onEnter(DisarmedSprintState &, const WeaponStateEvent & e)
{
    ArmedChangeStateBase::equipWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.armed.run);

}

void RunArmedChangeState::onEnter(ArmedSprintState &, const WeaponStateEvent &)
{
    ArmedChangeStateBase::disarmWeapon();
    MoveStateBase::changeAnimationClips(context_.animClipNames.disarmed.run);

}

void RunArmedChangeState::update(float dt)
{
    MoveStateBase::update(dt);
}

void RunArmedChangeState::update(const SprintStateChangeEvent& event)
{
    ArmedChangeStateBase::update(event);
}

void RunArmedChangeState::update(const WeaponStateEvent & e)
{
    ArmedChangeStateBase::update(e);
    onEnter();
}
}  // namespace Components
}  // namespace GameEngine
