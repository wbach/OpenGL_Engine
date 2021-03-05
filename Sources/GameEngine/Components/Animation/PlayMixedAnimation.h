#pragma once
#include "Context.h"
#include "GameEngine/Animations/AnimationClip.h"
#include "IState.h"

namespace GameEngine
{
namespace Components
{
class PlayMixedAnimation : public IState
{
public:
    PlayMixedAnimation(Context&);

    bool update(float) override;
    const std::string& getAnimationClipName() const override;

    void handle(const ChangeAnimationEvent&) override;
    void handle(const StopAnimationEvent&) override;

private:
    void increaseAnimationTime(float deltaTime);

private:
    Context& context_;

    struct Group
    {
        const Animation::AnimationClip& clip_;
        float time_;
        float playingSpeed_;
        const std::vector<std::function<void()>>& endCallbacks_;
        std::vector<uint32> joints_;
    };

    std::string animationName_;
    std::unordered_map<std::string, Group> groups_;
};
}  // namespace Components
}  // namespace GameEngine
