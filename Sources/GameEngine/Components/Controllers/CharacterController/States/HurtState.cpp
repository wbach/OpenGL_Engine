#include "HurtState.h"

#include "../FsmContext.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"

namespace GameEngine
{
namespace Components
{
HurtState::HurtState(FsmContext& context)
    : BaseState(context)
{
}
bool HurtState::entryCondition() const
{
    return not context_.animClipNames.disarmed.posture.stand.hurt.empty() or
           not context_.animClipNames.armed.posture.stand.hurt.empty() or
           not context_.animClipNames.disarmed.posture.crouched.hurt.empty() or
           not context_.animClipNames.armed.posture.crouched.hurt.empty();
}
void HurtState::setArmedRelatedClip()
{
    ChangeAnimationAndSubForEnd(context_.animClipNames.armed.posture.stand.hurt);
}

void HurtState::setDisarmedRelatedClip()
{
    ChangeAnimationAndSubForEnd(context_.animClipNames.disarmed.posture.stand.hurt);
}

void HurtState::setCrouchArmedRelatedClip()
{
    ChangeAnimationAndSubForEnd(context_.animClipNames.armed.posture.crouched.hurt);
}

void HurtState::setCrouchDisarmedRelatedClip()
{
    ChangeAnimationAndSubForEnd(context_.animClipNames.disarmed.posture.crouched.hurt);
}

void HurtState::setArmedChangeRelatedClip()
{
    if (context_.weaponArmedChangeState == FsmContext::WeaponArmedChangeState::Equip)
    {
        setDisarmedRelatedClip();
    }
    else
    {
        setArmedRelatedClip();
    }
}

void HurtState::setArmedChangeInCrouchRelatedClip()
{
    if (context_.weaponArmedChangeState == FsmContext::WeaponArmedChangeState::Equip)
    {
        setCrouchDisarmedRelatedClip();
    }
    else
    {
        setCrouchArmedRelatedClip();
    }
}
void HurtState::ChangeAnimationAndSubForEnd(const std::string& animName)
{
    if (animName.empty())
    {
        LOG_DEBUG << "animName is empty";
        context_.characterController.pushEventToQueue(AnimationPlayEndEvent{});
        return;
    }

    animationSubId =
        context_.animator.SubscribeForAnimationFrame(animName,
                                                     [this]()
                                                     {
                                                         animationSubId.reset();
                                                         context_.characterController.pushEventToQueue(AnimationPlayEndEvent{});
                                                     });

    context_.animator.ChangeAnimation(animName);
}
void HurtState::onLeave()
{
    flushEvents();
}
}  // namespace Components
}  // namespace GameEngine
