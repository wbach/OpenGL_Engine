#include "AnimationStateBase.h"

#include <Logger/Log.h>

#include "AnimationClipInfo.h"

namespace GameEngine
{
namespace Components
{
void AnimationStateBase::notifyFrameSubsribers(const AnimationClipInfo& clipInfo,
                                               const Animation::KeyFrame* currentFrame, float time,
                                               float& previousFrameTimeStamp)
{
    if (not currentFrame or clipInfo.subscribers.empty())
    {
        return;
    }
    // TO DO: Remove workaround
    if (time > clipInfo.clip.GetLength())
    {
        currentFrame = &clipInfo.clip.GetFrames().back();
    }

    // Unsubscribe during callbacks
    auto tmpSubscirbers = clipInfo.subscribers;
    for (const auto& sub : tmpSubscirbers)
    {
        if (compare(sub.timeStamp, currentFrame->timeStamp) and
            not compare(currentFrame->timeStamp, previousFrameTimeStamp))
        {
            sub.callback();
        }
    }

    previousFrameTimeStamp = currentFrame->timeStamp;
}

bool AnimationStateBase::increaseAnimationTime(float& currentTime, float& previousFrameTimeStamp,
                                               const AnimationClipInfo& clipInfo,
                                               Animation::KeyFrame const* currentFrame, float deltaTime)
{
    auto dir = clipInfo.playDirection == PlayDirection::forward ? 1.f : -1.f;
    currentTime += deltaTime * clipInfo.playSpeed * dir;

    notifyFrameSubsribers(clipInfo, currentFrame, currentTime, previousFrameTimeStamp);

    if (currentTime > clipInfo.clip.GetLength())
    {
        if (clipInfo.clip.playType == Animation::AnimationClip::PlayType::once)
        {
            return false;
        }
        currentTime = fmodf(currentTime, clipInfo.clip.GetLength());
    }
    if (currentTime < 0)
    {
        currentTime = clipInfo.clip.GetLength() + currentTime;
    }
    return true;
}
bool AnimationStateBase::isAnimationPlaying(const std::string&) const
{
    return false;
}
}  // namespace Components
}  // namespace GameEngine
