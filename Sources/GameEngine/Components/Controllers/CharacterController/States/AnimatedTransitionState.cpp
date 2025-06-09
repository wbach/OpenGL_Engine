#include "AnimatedTransitionState.h"

#include "../CharacterController.h"
#include "../FsmContext.h"
#include "CharacterControllerCommonDefs.h"

namespace GameEngine
{
namespace Components
{
AnimatedTransitionState::AnimatedTransitionState(FsmContext &context)
    : BaseState{context}
{
}

void AnimatedTransitionState::onEnter(DisarmedRunState &prevState, DisarmedIdleState &targetState)
{
    const std::string stopRunAnimName{"StandingRunForwardStopRootMontion"};
    setAnimAndSubscribeForEnd(stopRunAnimName);
    targetStateVariant = &targetState;
}

void AnimatedTransitionState::onEnter(ArmedRunState &, ArmedIdleState & targetState)
{
    const std::string stopRunAnimName{"StandingRunForwardStopRootMontion"};
    setAnimAndSubscribeForEnd(stopRunAnimName);
    targetStateVariant = &targetState;
}

AnimatedTransitionState::MaybeAttackStates AnimatedTransitionState::handle(const AnimatedTransitionEndEvent &)
{
    if (std::holds_alternative<DisarmedIdleState *>(targetStateVariant))
    {
        return Utils::StateMachine::TransitionTo<DisarmedIdleState>{};
    }
    if (std::holds_alternative<ArmedIdleState *>(targetStateVariant))
    {
        return Utils::StateMachine::TransitionTo<ArmedIdleState>{};
    }
    return Utils::StateMachine::Nothing{};
}

void AnimatedTransitionState::setAnimAndSubscribeForEnd(const std::string &animName)
{
    if (animSubId)
    {
        DEBUG_LOG("Dodge already performed!");
        return;
    }

    setAnim(animName);
    animSubId = context_.animator.SubscribeForAnimationFrame(
        animName,
        [&]()
        {
            context_.characterController.pushEventToFrontQueue(AnimatedTransitionEndEvent{});
            animSubId.reset();
        });
}

void AnimatedTransitionState::setAnim(const std::string &animName)
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
