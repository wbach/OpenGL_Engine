#pragma once
#include <unordered_map>

#include "Context.h"
#include "GameEngine/Animations/AnimationClip.h"
#include "AnimationStateBase.h"

namespace GameEngine
{
namespace Components
{
class PlayMixedAnimation : public AnimationStateBase
{
private:
    struct Group
    {
        const AnimationClipInfo& clipInfo;
        float time;
        float direction;
        std::vector<uint32> jointIds;
        CurrentFrames frames{nullptr, nullptr};
        float previousFrameTimeStamp{-1.0f};
    };

public:
    PlayMixedAnimation(Context&, const AnimationClipInfoPerGroup&);

    bool update(float) override;

    void handle(const ChangeAnimationEvent&) override;
    void handle(const StopAnimationEvent&) override;

    std::vector<std::string> getCurrentAnimation() const override;

private:
    void increaseAnimationTime(float deltaTime);

private:
    Context& context_;

    std::unordered_map<std::string, Group> groups_;
};
}  // namespace Components
}  // namespace GameEngine
