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
    animName = context_.animClipNames.disarmed.dodgeDive;

    setAnim();
    dodgeAnimSubId = context_.animator.SubscribeForAnimationFrame(
        animName, [&]() { context_.characterController.pushEventToFrontQueue(DodgeEndEvent{}); });
}

void DodgeState::onEnter(DisarmedFallingState &)
{
    DEBUG_LOG("perform DisarmedFallingState");
    context_.characterController.pushEventToFrontQueue(DodgeEndEvent{});
}

void DodgeState::onLeave(const DodgeEndEvent &)
{
    DEBUG_LOG("perform DodgeEndEvent");
    flushEvents();
}

void DodgeState::setAnim()
{
    if (not animName.empty())
    {
        DEBUG_LOG(animName);
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
