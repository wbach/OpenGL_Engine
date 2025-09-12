#include "DodgeState.h"

#include "../CharacterController.h"
#include "../FsmContext.h"
#include "CharacterControllerCommonDefs.h"
#include "GameEngine/Components/Physics/CapsuleShape.h"
#include "GameEngine/Physics/CollisionContactInfo.h"

namespace GameEngine
{
namespace Components
{
DodgeState::DodgeState(FsmContext &context)
    : BaseState{context}
{
}

void DodgeState::onEnter(const DodgeDiveEvent &event)
{
    setAnimAndSubscribeForEnd(context_.animClipNames.disarmed.dodgeDive);
}

void DodgeState::onEnter(const DodgeForwardEvent &e)
{
    setAnimAndSubscribeForEnd(context_.animClipNames.disarmed.dodge.forward);
}

void DodgeState::onEnter(const DodgeBackwardEvent &e)
{
    setAnimAndSubscribeForEnd(context_.animClipNames.disarmed.dodge.backward);
}

void DodgeState::onEnter(const DodgeRightEvent &e)
{
    setAnimAndSubscribeForEnd(context_.animClipNames.disarmed.dodge.right);
}

void DodgeState::onEnter(const DodgeLeftEvent &e)
{
    setAnimAndSubscribeForEnd(context_.animClipNames.disarmed.dodge.left);
}

void DodgeState::onEnter(DisarmedFallingState &)
{
    context_.characterController.pushEventToFrontQueue(DodgeEndEvent{});
}

void DodgeState::onLeave(const DodgeEndEvent &)
{
    flushEvents();
}

void DodgeState::setAnimAndSubscribeForEnd(const std::string &animName)
{
    if (dodgeAnimSubId)
    {
        return;
    }

    setAnim(animName);
    dodgeAnimSubId =
        context_.animator.SubscribeForAnimationFrame(animName,
                                                     [&]()
                                                     {
                                                         context_.characterController.pushEventToFrontQueue(DodgeEndEvent{});
                                                         dodgeAnimSubId.reset();
                                                     });
}

void DodgeState::setAnim(const std::string &animName)
{
    if (not animName.empty())
    {
        const auto &currentClips = context_.animator.getCurrentAnimationName();

        auto iter = std::find(currentClips.begin(), currentClips.end(), animName);

        if (iter == currentClips.end())
        {
            context_.animator.ChangeAnimation(animName, Animator::AnimationChangeType::smooth, PlayDirection::forward,
                                              std::nullopt);
        }
    }
}
}  // namespace Components
}  // namespace GameEngine
