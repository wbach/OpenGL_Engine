#include "AnimationStateBase.h"

#include <Logger/Log.h>

#include "AnimationClipInfo.h"

namespace GameEngine
{
namespace Components
{
AnimationStateBase::AnimationStateBase(Context& context)
    : context_{context}
{
    rootMontionVec_.reset();
}

void AnimationStateBase::notifyFrameSubsribers(const AnimationClipInfo& clipInfo, const Animation::KeyFrame* currentFrame,
                                               float time, float& previousFrameTimeStamp)
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
        if (compare(sub.timeStamp, currentFrame->timeStamp.value) and
            not compare(currentFrame->timeStamp.value, previousFrameTimeStamp))
        {
            sub.callback();
        }
    }

    previousFrameTimeStamp = currentFrame->timeStamp.value;
}

bool AnimationStateBase::increaseAnimationTime(float& currentTime, float& previousFrameTimeStamp,
                                               const AnimationClipInfo& clipInfo, Animation::KeyFrame const* currentFrame,
                                               float deltaTime)
{
    auto dir = clipInfo.playDirection == PlayDirection::forward ? 1.f : -1.f;
    currentTime += deltaTime * clipInfo.playSpeed * dir;

    notifyFrameSubsribers(clipInfo, currentFrame, currentTime, previousFrameTimeStamp);

    if (currentTime > clipInfo.clip.GetLength())
    {
        if (clipInfo.playType == Components::AnimationClipInfo::PlayType::once)
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
void AnimationStateBase::calculateRootMontionVecAndClearTranslation()
{
    auto& currentPose = context_.currentPose;
    if (context_.montionRootJointId)
    {
        auto currentPoseRootJointIter = currentPose.data.find(*context_.montionRootJointId);
        if (currentPoseRootJointIter != currentPose.data.end())
        {
            auto& [_, data] = *currentPoseRootJointIter;

            context_.moveVectorForRootMontion = vec3{0.f};
            if (not rootMontionVec_)
            {
                context_.moveVectorForRootMontion = data.transform.position;
            }
            else
            {
                context_.moveVectorForRootMontion = data.transform.position - (*rootMontionVec_);
            }
            rootMontionVec_ = data.transform.position;

            // clear MoveVec from animation
            data.matrix[3][0] = 0;
            data.matrix[3][2] = 0;

            data.transform.position.x = 0;
            data.transform.position.z = 0;
        }
    }
}
}  // namespace Components
}  // namespace GameEngine
