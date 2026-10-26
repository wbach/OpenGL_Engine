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

void DeathState::onEnter()
{
    if (auto rb = context_.gameObject.GetComponent<Rigidbody>())
    {
        rb->Deactivate();
    }
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

void DeathState::setArmedChangeRelatedClip()
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

void DeathState::setArmedChangeInCrouchRelatedClip()
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

}  // namespace Components
}  // namespace GameEngine
