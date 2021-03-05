#include "AnimationTransitionGrouped.h"

#include "EmptyState.h"
#include "PlayMixedAnimation.h"
#include "StateMachine.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const std::string clipName{"AnimationTransitionGrouped"};
}

AnimationTransitionGrouped::AnimationTransitionGrouped(Context &context, const AnimationClipInfo &currentInfo,
                                                       float currentTime, const ChangeAnimationEvent &event)
    : context_{context}
    , currentClipInfo_{currentInfo}
    , currentClipProgres_{currentTime}
    , secondaryClipStartupTime_{event.startTime}
    , secondaryClipInfo_{event.info}
//, secondaryAnimJointGroup_{event.jointGroupName}
{
}

bool AnimationTransitionGrouped::update(float deltaTime)
{
    calculateCurrentAnimationPose(context_.currentPose, currentClipInfo_.clip, currentClipProgres_,
                                  currentAnimJointGroup_);

    interpolatePoses(context_.currentPose, startChaneAnimKeyFrame_, endChangeAnimKeyFrame_, transitionProgress_);

    increaseAnimationTime(deltaTime);
    increaseTransitionTime(deltaTime);
    return true;
}

const std::string &AnimationTransitionGrouped::getAnimationClipName() const
{
    return clipName;
}

void AnimationTransitionGrouped::handle(const ChangeAnimationEvent&)
{
}

void AnimationTransitionGrouped::handle(const StopAnimationEvent &)
{
    context_.machine.transitionTo(std::make_unique<EmptyState>(context_));
}

void AnimationTransitionGrouped::increaseAnimationTime(float deltaTime)
{
    currentClipProgres_ += deltaTime * currentClipInfo_.playSpeed;  //* direction_;

    if (currentClipProgres_ > currentClipInfo_.clip.GetLength())
    {
        currentClipProgres_ = fmodf(currentClipProgres_, currentClipInfo_.clip.GetLength());
    }
    if (currentClipProgres_ < 0)
    {
        currentClipProgres_ = currentClipInfo_.clip.GetLength() + currentClipProgres_;
    }
}

void AnimationTransitionGrouped::increaseTransitionTime(float deltaTime)
{
    transitionProgress_ += (1.f / timeForChange_) * deltaTime;

    if (transitionProgress_ > 1.f)
    {
        context_.machine.transitionTo(std::make_unique<PlayMixedAnimation>(context_));
        return;
    }
}
}  // namespace Components
}  // namespace GameEngine
