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
    Context& context_;

    std::unordered_map<std::string, PlayGroup> groups_;
};
}  // namespace Components
}  // namespace GameEngine
