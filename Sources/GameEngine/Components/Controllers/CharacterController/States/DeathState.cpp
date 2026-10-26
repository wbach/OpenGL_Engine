#include "DeathState.h"

#include "../FsmContext.h"
#include "GameEngine/Components/Controllers/CharacterController/States/HurtState.h"
#include "GameEngine/Components/Physics/Rigidbody.h"

namespace GameEngine
{
namespace Components
{
DeathState::DeathState(FsmContext& context)
    : BaseState{context}
{
}

void DeathState::onEnter(HurtState&, const DeathEvent&)
{
    setAnim(context_.animClipNames.disarmed.posture.stand.death);
}

void DeathState::onEnter(ArmedAttackState&, const DeathEvent&)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(DisarmedAttackState&, const DeathEvent&)
{
    setAnim(context_.animClipNames.disarmed.posture.stand.death);
}

void DeathState::onEnter(DisarmedAttackAndRunState&, const DeathEvent&)
{
    setAnim(context_.animClipNames.disarmed.posture.stand.death);
}

void DeathState::onEnter(DisarmedAttackAndWalkState&, const DeathEvent&)
{
    setAnim(context_.animClipNames.disarmed.posture.stand.death);
}

void DeathState::onEnter(DisarmedAttackAndWalkAndRotateState&, const DeathEvent&)
{
    setAnim(context_.animClipNames.disarmed.posture.stand.death);
}

void DeathState::onEnter(IdleArmedChangeState&, const DeathEvent&)
{
    setAnim(context_.animClipNames.disarmed.posture.stand.death);
}


void DeathState::onEnter(RotateArmedChangeState&, const DeathEvent&)
{
    setAnim(context_.animClipNames.disarmed.posture.stand.death);
}

void DeathState::onEnter(RunArmedChangeState&, const DeathEvent&)
{
    setAnim(context_.animClipNames.disarmed.posture.stand.death);
}

void DeathState::onEnter(RunAndRotateArmedChangeState&, const DeathEvent&)
{
    setAnim(context_.animClipNames.disarmed.posture.stand.death);
}

void DeathState::onEnter(WalkArmedChangeState&, const DeathEvent&)
{
    setAnim(context_.animClipNames.disarmed.posture.stand.death);
}

void DeathState::onEnter(WalkAndRotateArmedChangeState&, const DeathEvent&)
{
    setAnim(context_.animClipNames.disarmed.posture.stand.death);
}

void DeathState::onEnter(DisarmedWalkState&, const DeathEvent&)
{
    setAnim(context_.animClipNames.disarmed.posture.stand.death);
}

void DeathState::onEnter(DisarmedWalkAndRotateState&, const DeathEvent&)
{
    setAnim(context_.animClipNames.disarmed.posture.stand.death);
}

void DeathState::onEnter(DisarmedSprintState&, const DeathEvent&)
{
    setAnim(context_.animClipNames.disarmed.posture.stand.death);
}


void DeathState::onEnter(ArmedRunState&, const DeathEvent&)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(ArmedRotateState&, const DeathEvent&)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(ArmedRunAndRotateState&, const DeathEvent&)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(ArmedWalkState&, const DeathEvent&)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(ArmedWalkAndRotateState&, const DeathEvent&)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(ArmedSprintState&, const DeathEvent&)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(ArmedSprintAndRotateState&, const DeathEvent&)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(JumpState&, const DeathEvent&)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(DodgeState&, const DeathEvent&)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(RecoilState&, const DeathEvent&)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(RecoilRotateState&, const DeathEvent&)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(RecoilWalkState&, const DeathEvent&)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(RecoilWalkAndRotateState&, const DeathEvent&)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(DrawArrowState&, const DeathEvent&)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(DrawArrowRotateState&, const DeathEvent&)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(DrawArrowWalkState&, const DeathEvent&)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(DrawArrowWalkAndRotateState&, const DeathEvent&)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}


void DeathState::onEnter(ArmedFallingState&, const DeathEvent&)
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::onEnter(DisarmedFallingState&, const DeathEvent&)
{
    setAnim(context_.animClipNames.disarmed.posture.stand.death);
}

void DeathState::setAnim(const std::string& animName)
{
    if (not animName.empty())
    {
        context_.animator.ChangeAnimation(animName, Animator::AnimationChangeType::smooth, PlayDirection::forward, std::nullopt);
    }
}

void DeathState::setArmedRelatedClip()
{
    setAnim(context_.animClipNames.armed.posture.stand.death);
}

void DeathState::setDisarmedRelatedClip()
{
    setAnim(context_.animClipNames.disarmed.posture.stand.death);
}

void DeathState::setCrouchArmedRelatedClip()
{
    setAnim(context_.animClipNames.armed.posture.crouched.death);
}

void DeathState::setCrouchDisarmedRelatedClip()
{
    setAnim(context_.animClipNames.disarmed.posture.crouched.death);
}

void DeathState::onEnter()
{
    if (auto rb = context_.gameObject.GetComponent<Rigidbody>())
    {
        rb->Deactivate();
    }
}
}  // namespace Components
}  // namespace GameEngine
