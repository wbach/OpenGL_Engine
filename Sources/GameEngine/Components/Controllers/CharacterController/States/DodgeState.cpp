#include "DodgeState.h"

#include "../CharacterController.h"
#include "../FsmContext.h"
#include "CharacterControllerCommonDefs.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "Types.h"
namespace GameEngine
{
namespace Components
{
DodgeState::DodgeState(FsmContext& context)
    : BaseState{context}
{
}

void DodgeState::onEnter(const DodgeDiveEvent& event)
{
    onEnterDodge(context_.animClipNames.disarmed.dodgeDive);
}

void DodgeState::onEnter(const DodgeForwardEvent& e)
{
    onEnterDodge(context_.animClipNames.disarmed.dodge.forward);
}

void DodgeState::onEnter(const DodgeBackwardEvent& e)
{
    onEnterDodge(context_.animClipNames.disarmed.dodge.backward);
}

void DodgeState::onEnter(const DodgeRightEvent& e)
{
    onEnterDodge(context_.animClipNames.disarmed.dodge.right);
}

void DodgeState::onEnter(const DodgeLeftEvent& e)
{
    onEnterDodge(context_.animClipNames.disarmed.dodge.left);
}

void DodgeState::onEnterDodge(const std::string& animName)
{
    if (auto info = context_.animator.getAnimationClipInfo(animName))
    {
        useRootMontion = info->rootMontion;
    }
    setAnimAndSubscribeForEnd(animName);
}

void DodgeState::onEnter(DisarmedFallingState&)
{
    context_.characterController.pushEventToFrontQueue(DodgeEndEvent{});
}

void DodgeState::onLeave(const DodgeEndEvent&)
{
    flushEvents();

    // Damp velocity to prevent sliding after dodge ends
    auto& rigidbody = context_.rigidbody;
    auto currentVel = rigidbody.GetVelocity();
    rigidbody.SetVelocity(currentVel * 0.1f);
}

void DodgeState::setAnimAndSubscribeForEnd(const std::string& animName)
{
    if (dodgeAnimSubId)
    {
        return;
    }

    setAnim(animName);
    dodgeAnimSubId =
        context_.animator.SubscribeForAnimationFrame(animName,
                                                     [this]()
                                                     {
                                                         context_.characterController.pushEventToFrontQueue(DodgeEndEvent{});
                                                         dodgeAnimSubId.reset();
                                                     });
}

void DodgeState::setAnim(const std::string& animName)
{
    if (not animName.empty())
    {
        const auto& currentClips = context_.animator.getCurrentAnimationName();

        auto iter = std::find(currentClips.begin(), currentClips.end(), animName);

        if (iter == currentClips.end())
        {
            context_.animator.ChangeAnimation(animName, Animator::AnimationChangeType::smooth, PlayDirection::forward,
                                              std::nullopt);
        }
    }
}
void DodgeState::update(float deltaTime)
{
    if (not useRootMontion)
    {
        auto& rigidbody = context_.rigidbody;
        auto currentVel = rigidbody.GetVelocity();
        auto direction  = rigidbody.GetRotation() * VECTOR_FORWARD;
        auto dodgeVel = direction * 5.f;

        if (currentVel.y < dodgeVel.y)
        {
            dodgeVel.y = currentVel.y;
        }

        rigidbody.SetVelocity(dodgeVel);
    }
}
}  // namespace Components
}  // namespace GameEngine
