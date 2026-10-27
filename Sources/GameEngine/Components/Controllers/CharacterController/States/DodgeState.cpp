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

DodgeState::~DodgeState()
{
    unsubscribeAnimation();
}

void DodgeState::onEnter(const DodgeDiveEvent& event)
{
    onEnterDodge(context_.animClipNames.disarmed.dodgeDive, context_.moveController.getCurrentDir());
}

void DodgeState::onEnter(const DodgeForwardEvent& e)
{
    onEnterDodge(context_.animClipNames.disarmed.dodge.forward, VECTOR_FORWARD);
}

void DodgeState::onEnter(const DodgeBackwardEvent& e)
{
    onEnterDodge(context_.animClipNames.disarmed.dodge.backward, VECTOR_BACKWARD);
}

void DodgeState::onEnter(const DodgeRightEvent& e)
{
    onEnterDodge(context_.animClipNames.disarmed.dodge.right, VECTOR_RIGHT);
}

void DodgeState::onEnter(const DodgeLeftEvent& e)
{
    onEnterDodge(context_.animClipNames.disarmed.dodge.left, VECTOR_LEFT);
}

void DodgeState::onEnterDodge(const std::string& animName, const vec3& direction)
{
    dodgeDirection_ = direction;

    if (auto info = context_.animator.getAnimationClipInfo(animName))
    {
        useRootMontion = info->rootMontion;
    }
    setAnimAndSubscribeForEnd(animName);
}

void DodgeState::onEnter(FallingState&)
{
    context_.characterController.pushEventToFrontQueue(DodgeEndEvent{});
}

void DodgeState::onLeave()
{
    unsubscribeAnimation();
}

void DodgeState::onLeave(const DodgeEndEvent&)
{
    flushEvents();

    if (not useRootMontion)
    {
        // Damp velocity to prevent sliding after dodge ends
        auto& rigidbody = context_.rigidbody;
        auto currentVel = rigidbody.GetVelocity();
        rigidbody.SetVelocity(currentVel * 0.1f);
        if (glm::length(vec2(currentVel.x, currentVel.z)) > 0.001f)
        {
            rigidbody.SetVelocity(vec3(0.0f, currentVel.y, 0.0f));
        }
    }

    unsubscribeAnimation();
}

void DodgeState::unsubscribeAnimation()
{
    if (dodgeAnimSubId)
    {
        return;
        context_.animator.UnSubscribeForAnimationFrame(*dodgeAnimSubId);
        dodgeAnimSubId.reset();
    }
}

void DodgeState::setAnimAndSubscribeForEnd(const std::string& animName)
{
    unsubscribeAnimation();

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
        auto direction  = rigidbody.GetRotation() * dodgeDirection_;
        auto dodgeVel   = direction * context_.dodgeSpeed;

        if (currentVel.y < dodgeVel.y)
        {
            dodgeVel.y = currentVel.y;
        }
        dodgeVel.y = currentVel.y;

        rigidbody.SetVelocity(dodgeVel);
    }
}
}  // namespace Components
}  // namespace GameEngine
