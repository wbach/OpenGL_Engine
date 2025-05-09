#pragma once
#include "AnimationClipInfo.h"
#include "Context.h"
#include "CurrentGroupsPlayingInfo.h"
#include "AnimationStateBase.h"

namespace GameEngine
{
namespace Components
{
struct ChangeAnimationEvent;

class AnimationTransitionMixedToSingle : public AnimationStateBase
{
public:
    AnimationTransitionMixedToSingle(Context&, const CurrentGroupsPlayingInfo&);
    bool update(float) override;

    void handle(const ChangeAnimationEvent&) override;
    void handle(const StopAnimationEvent&) override;

     std::vector<std::string> getCurrentAnimation() const override;
     bool isAnimationPlaying(const std::string&) const override;

private:
    void increaseAnimationTime(float);
    void increaseTransitionTime(float);

private:
    PlayGroup currentAnim;

    struct TransitionGroups
    {
        const std::vector<uint32>& jointGroups;
    };

    float transitionProgress_;
    Animation::KeyFrame startChangeAnimKeyFrame_;
    std::vector<TransitionGroups> transitionGroups_;
};
}  // namespace Components
}  // namespace GameEngine
