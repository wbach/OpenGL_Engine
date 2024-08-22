#include "DeathState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
DeathState::DeathState(FsmContext &context)
    : context_{context}
{
}

void DeathState::update(float)
{
}

void DeathState::onEnter(AimCrouchWalkState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.crouched.death);
}

void DeathState::onEnter(AimCrouchRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.crouched.death);
}

void DeathState::onEnter(AimCrouchWalkAndRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.crouched.death);
}

void DeathState::onEnter(CrouchWalkArmedChangeState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.disarmed.posture.crouched.death);
}

void DeathState::onEnter(CrouchWalkAndRotateArmedChangeState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.disarmed.posture.crouched.death);
}

void DeathState::onEnter(CrouchRotateArmedChangeState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.disarmed.posture.crouched.death);
}

void DeathState::onEnter(DisarmedCrouchWalkAndRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.disarmed.posture.crouched.death);
}

void DeathState::onEnter(ArmedCrouchWalkAndRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.crouched.death);
}

void DeathState::onEnter(ArmedCrouchRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.crouched.death);
}

void DeathState::onEnter(ArmedAttackState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(DisarmedAttackState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.disarmed.posture.stand.death);
}

void DeathState::onEnter(DisarmedIdleCrouchState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.disarmed.posture.crouched.death);
}

void DeathState::onEnter(DisarmedAttackAndRunState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.disarmed.posture.stand.death);
}

void DeathState::onEnter(DisarmedAttackAndWalkState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.disarmed.posture.stand.death);
}

void DeathState::onEnter(DisarmedAttackAndRunAndRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.disarmed.posture.stand.death);
}

void DeathState::onEnter(DisarmedAttackAndWalkAndRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.disarmed.posture.stand.death);
}

void DeathState::onEnter(IdleArmedChangeState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.disarmed.posture.stand.death);
}

void DeathState::onEnter(IdleCrouchArmedChangeState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.disarmed.posture.crouched.death);
}

void DeathState::onEnter(RotateArmedChangeState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.disarmed.posture.stand.death);
}

void DeathState::onEnter(RunArmedChangeState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.disarmed.posture.stand.death);
}

void DeathState::onEnter(RunAndRotateArmedChangeState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.disarmed.posture.stand.death);
}

void DeathState::onEnter(WalkArmedChangeState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.disarmed.posture.stand.death);
}

void DeathState::onEnter(WalkAndRotateArmedChangeState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.disarmed.posture.stand.death);
}

void DeathState::onEnter(DisarmedIdleState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.disarmed.posture.stand.death);
}

void DeathState::onEnter(DisarmedRunState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.disarmed.posture.stand.death);
}

void DeathState::onEnter(DisarmedRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.disarmed.posture.stand.death);
}

void DeathState::onEnter(DisarmedCrouchRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.disarmed.posture.crouched.death);
}

void DeathState::onEnter(DisarmedRunAndRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.disarmed.posture.stand.death);
}

void DeathState::onEnter(DisarmedWalkState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.disarmed.posture.stand.death);
}

void DeathState::onEnter(DisarmedCrouchWalkState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.disarmed.posture.crouched.death);
}

void DeathState::onEnter(DisarmedWalkAndRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.disarmed.posture.stand.death);
}

void DeathState::onEnter(DisarmedSprintState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.disarmed.posture.stand.death);
}

void DeathState::onEnter(DisarmedSprintAndRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.disarmed.posture.stand.death);
}

void DeathState::onEnter(ArmedIdleState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(ArmedIdleCrouchState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.crouched.death);
}

void DeathState::onEnter(ArmedRunState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(ArmedRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(ArmedRunAndRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(ArmedWalkState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(ArmedWalkAndRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(ArmedCrouchWalkState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.crouched.death);
}

void DeathState::onEnter(ArmedSprintState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(ArmedSprintAndRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(ArmedAttackAndRunState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(JumpState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(AimState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(AimCrouchState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.crouched.death);
}

void DeathState::onEnter(AimRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(AimWalkState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(AimWalkAndRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(RecoilState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(RecoilCrouchState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.crouched.death);
}

void DeathState::onEnter(RecoilCrouchWalkState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.crouched.death);
}

void DeathState::onEnter(RecoilRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(RecoilWalkState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(RecoilWalkAndRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(RecoilCrouchRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.crouched.death);
}

void DeathState::onEnter(RecoilCrouchWalkAndRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.crouched.death);
}

void DeathState::onEnter(DrawArrowState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(DrawArrowCrouchWalkState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.crouched.death);
}

void DeathState::onEnter(DrawArrowCrouchState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.crouched.death);
}

void DeathState::onEnter(DrawArrowRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(DrawArrowWalkState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(DrawArrowWalkAndRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(DrawArrowCrouchRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.crouched.death);
}

void DeathState::onEnter(DrawArrowCrouchWalkAndRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.crouched.death);
}

void DeathState::onEnter(DisarmedAttackAndRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.disarmed.posture.stand.death);
}

void DeathState::onEnter(ArmedAttackAndWalkState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(ArmedAttackAndRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(ArmedAttackAndRunAndRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(ArmedAttackAndWalkAndRotateState &, const DeathEvent &)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::setAnim(const std::string &animName)
{
    if (not animName.empty())
    {
        context_.animator.ChangeAnimation(animName, Animator::AnimationChangeType::smooth, PlayDirection::forward, std::nullopt);
    }
}

}  // namespace Components
}  // namespace GameEngine
