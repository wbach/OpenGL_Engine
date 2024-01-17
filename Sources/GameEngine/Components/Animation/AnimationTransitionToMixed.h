#pragma once
#include "AnimationClipInfo.h"
#include "Context.h"
#include "CurrentGroupsPlayingInfo.h"
#include "IState.h"

namespace GameEngine
{
namespace Components
{
struct ChangeAnimationEvent;

class AnimationTransitionToMixed : public IState
{
public:
    AnimationTransitionToMixed(Context&, const std::vector<CurrentGroupsPlayingInfo>&, const ChangeAnimationEvent&,
                               std::function<void()> = nullptr);
    bool update(float) override;

    void handle(const ChangeAnimationEvent&) override;
    void handle(const StopAnimationEvent&) override;

    std::vector<std::string> getCurrentAnimation() const override;

private:
    void increaseAnimationTime(float);
    void increaseTransitionTime(float);

private:
    Context& context_;

    struct Group
    {
        const AnimationClipInfo& clipInfo_;
        float progres_;
        const std::vector<uint32>& jointGroup_;
        std::function<void()> onTransitionEnd_;
    };
    std::unordered_map<std::string, Group> currentGroups_;

    struct TransitionGroup
    {
        float startupTime_;
        const AnimationClipInfo& clipInfo_;
        const std::vector<uint32>& jointGroup_;
        std::function<void()> onTransitionEnd_;
        Animation::KeyFrame startKeyFrame_;
        float timeForChange_;
        float progress_;
    };
    std::unordered_map<std::string, TransitionGroup> transtionGroups_;
};
}  // namespace Components
}  // namespace GameEngine
