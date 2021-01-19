#include "PlayAnimationEvent.h"

#include "PlayAnimation.h"
#include "StateMachine.h"
#include <Logger/Log.h>

namespace GameEngine
{
namespace Components
{
PlayAnimationEvent::PlayAnimationEvent(const AnimationPlayingInfo& animationPlayingInfo)
    : animationPlayingInfo_{animationPlayingInfo}
{
}
PlayAnimationEvent::PlayAnimationEvent(const Animation::AnimationClip& clip)
    : PlayAnimationEvent(clip, 1.f, PlayDirection::forward)
{
}

PlayAnimationEvent::PlayAnimationEvent(const Animation::AnimationClip& clip, float playSpeed,
                                       PlayDirection playDirection)
    : PlayAnimationEvent(clip, playSpeed, playDirection, {})
{
}
PlayAnimationEvent::PlayAnimationEvent(const Animation::AnimationClip& clip, float playSpeed,
                                       PlayDirection playDirection, const std::vector<std::function<void()>>& endCallbacks)
    : animationPlayingInfo_{clip, playSpeed, playDirection, endCallbacks}
{
    DEBUG_LOG("PlayAnimationEvent process clip.name=" + clip.name);
}
void PlayAnimationEvent::process()
{
    DEBUG_LOG("PlayAnimationEvent process clip.name=" + animationPlayingInfo_.clip.name);
    machine->currentState_ = std::make_unique<PlayAnimation>(*this);
}
}  // namespace Components
}  // namespace GameEngine
