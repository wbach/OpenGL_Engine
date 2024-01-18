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

    DEBUG_LOG("notifyFrameSubsribers time = " + std::to_string(time) + " / " +
              std::to_string(clipInfo.clip.GetLength()));

    // TO DO: Remove workaround
    if (time > clipInfo.clip.GetLength())
    {
        DEBUG_LOG("Workaround set last frame if over time");
        currentFrame = &clipInfo.clip.GetFrames().back();
    }

    // Unsubscribe during callbacks
    auto tmpSubscirbers = clipInfo.subscribers;
    for (const auto& sub : tmpSubscirbers)
    {
        if (compare(sub.timeStamp, currentFrame->timeStamp) and
            not compare(currentFrame->timeStamp, previousFrameTimeStamp))
        {
            DEBUG_LOG("notifyFrameSubsribers");
            sub.callback();
            DEBUG_LOG("notifyFrameSubsribers end");
        }
    }

    previousFrameTimeStamp = currentFrame->timeStamp;
}

bool AnimationStateBase::increaseAnimationTime(float& currentTime, float& previousFrameTimeStamp, const AnimationClipInfo& clipInfo,
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
}  // namespace Components
}  // namespace GameEngine
