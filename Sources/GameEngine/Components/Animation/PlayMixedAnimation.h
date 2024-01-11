#pragma once
#include "Context.h"
#include "GameEngine/Animations/AnimationClip.h"
#include "IState.h"
#include <unordered_map>

namespace GameEngine
{
namespace Components
{
class PlayMixedAnimation : public IState
{
public:
    PlayMixedAnimation(Context&, const AnimationClipInfoPerGroup&);

    bool update(float) override;

    void handle(const ChangeAnimationEvent&) override;
    void handle(const StopAnimationEvent&) override;

private:
    void increaseAnimationTime(float deltaTime);

private:
    Context& context_;

    struct Group
    {
        const AnimationClipInfo& clipInfo;
        float time;
        float direction;
        std::vector<uint32> jointIds;
    };

    std::unordered_map<std::string, Group> groups_;
};
}  // namespace Components
}  // namespace GameEngine
