#pragma once
#include <unordered_map>

#include "AnimationStateBase.h"
#include "Context.h"
#include "GameEngine/Animations/AnimationClip.h"

namespace GameEngine
{
namespace Components
{
class PlayMixedAnimation : public AnimationStateBase
{
public:
    PlayMixedAnimation(Context&, const AnimationClipInfoPerGroup&);

    bool update(float) override;

    void handle(const ChangeAnimationEvent&) override;
    void handle(const StopAnimationEvent&) override;

    std::vector<std::string> getCurrentAnimation() const override;
    bool isAnimationPlaying(const std::string&) const override;

private:
    void increaseAnimationTime(float deltaTime);

private:
    std::unordered_map<std::string, PlayGroupMixed> groups_;
};
}  // namespace Components
}  // namespace GameEngine
