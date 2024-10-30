#include "PlayAnimation.h"

#include <Logger/Log.h>

#include "AnimationTransition.h"
#include "AnimationTransitionToMixed.h"
#include "EmptyState.h"
#include "StateMachine.h"

namespace GameEngine
{
namespace Components
{
PlayAnimation::PlayAnimation(Context& context, const AnimationClipInfo& info, float startTime)
    : context_{context}
    , playInfo_{{.time = startTime, .clipInfo = info}, info.playDirection == PlayDirection::forward ? 1.f : -1.f}

{
}
bool PlayAnimation::update(float deltaTime)
{
    calculateCurrentAnimationPose(context_.currentPose, playInfo_.clipInfo.clip, playInfo_.time);

    if (not increaseAnimationTime(playInfo_.time, playInfo_.previousFrameTimeStamp, playInfo_.clipInfo,
                                  context_.currentPose.frames.first, deltaTime))
    {
        context_.machine.transitionTo<EmptyState>(context_);
    }

    return true;
}
void PlayAnimation::handle(const ChangeAnimationEvent& event)
{
    if (event.jointGroupName)
    {
        std::vector<CurrentGroupsPlayingInfo> v{{playInfo_.clipInfo, playInfo_.time, {}}};

        for (auto& [name, group] : context_.jointGroups)
        {
            if (name != event.jointGroupName)
            {
                v.front().jointGroupNames.push_back(name);
            }
        }
        context_.machine.transitionTo<AnimationTransitionToMixed>(context_, v, event);
    }
    else
    {
        context_.machine.transitionTo<AnimationTransition>(context_, event.info, event.startTime, event.onTransitionEnd);
    }
}

void PlayAnimation::handle(const StopAnimationEvent& e)
{
    context_.machine.transitionTo<EmptyState>(context_);
}

std::vector<std::string> PlayAnimation::getCurrentAnimation() const
{
    return {playInfo_.clipInfo.clip.getName()};
}

bool PlayAnimation::isAnimationPlaying(const std::string& name) const
{
    return playInfo_.clipInfo.clip.getName() == name;
}
}  // namespace Components
}  // namespace GameEngine
