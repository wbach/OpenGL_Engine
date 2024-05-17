#include "Common.h"

#include "GameEngine/Animations/AnimationUtils.h"
#include "GameEngine/Animations/JointTransform.h"

namespace GameEngine
{
namespace Components
{
void interpolatePoses(Pose& currentPose, const Animation::KeyFrame& previousFrame, const Animation::KeyFrame& nextFrame,
                      float progression)
{
    for (const auto& [jointId, previousTransform] : previousFrame.transforms)
    {
        const auto& nextFrameTransformIter = nextFrame.transforms.find(jointId);
        if (nextFrameTransformIter != nextFrame.transforms.cend())
        {
            const auto& nextTransform = nextFrameTransformIter->second;

            auto currentPoseJointIter = currentPose.data.find(jointId);
            if (currentPoseJointIter != currentPose.data.end())
            {
                auto& poseData = currentPoseJointIter->second;

                poseData.transform = Interpolate(previousTransform, nextTransform, progression);
                poseData.matrix    = GetLocalTransform(poseData.transform);
            }
            else
            {
                auto transform = Interpolate(previousTransform, nextTransform, progression);
                currentPose.data.insert({jointId, {transform, GetLocalTransform(transform)}});
            }
        }
    }
}

void interpolatePoses(Pose& currentPose, const Animation::KeyFrame& previousFrame, const Animation::KeyFrame& nextFrame,
                      float progression, const std::vector<uint32>& usedJointIds)
{
    for (const auto& [jointId, previousTransform] : previousFrame.transforms)
    {
        auto usedJointIdsIter = std::find(usedJointIds.begin(), usedJointIds.end(), jointId);
        if (usedJointIdsIter == usedJointIds.end())
            continue;

        const auto& nextFrameTransformIter = nextFrame.transforms.find(jointId);

        if (nextFrameTransformIter != nextFrame.transforms.cend())
        {
            auto& poseData            = currentPose.data[jointId];
            const auto& nextTransform = nextFrameTransformIter->second;
            poseData.transform        = Interpolate(previousTransform, nextTransform, progression);
            poseData.matrix           = GetLocalTransform(poseData.transform);
        }
    }
}

float calculateProgression(const Animation::KeyFrame& previousFrame, const Animation::KeyFrame& nextFrame, float time)
{
    float totalTime   = nextFrame.timeStamp.value - previousFrame.timeStamp.value;
    float currentTime = time - previousFrame.timeStamp.value;
    return glm::clamp(currentTime / totalTime, 0.f, 1.f);
}

CurrentFrames getPreviousAndNextFrames(const Animation::AnimationClip& clip, float time)
{
    const auto& allFrames = clip.GetFrames();

    auto previousFrame = &allFrames[0];
    auto nextFrame     = &allFrames[0];

    for (uint32 i = 1; i < allFrames.size(); i++)
    {
        nextFrame = &allFrames[i];
        if (nextFrame->timeStamp.value > time)
            break;

        previousFrame = &allFrames[i];
    }
    return {previousFrame, nextFrame};
}
void calculateCurrentAnimationPose(Pose& currentPose, const Animation::AnimationClip& clip, float time)
{
    if (clip.GetFrames().empty())
        return;

    currentPose.frames = getPreviousAndNextFrames(clip, time);
    float progression  = calculateProgression(*currentPose.frames.first, *currentPose.frames.second, time);
    interpolatePoses(currentPose, *currentPose.frames.first, *currentPose.frames.second, progression);
}
void calculateCurrentAnimationPose(Pose& currentPose, const Animation::AnimationClip& clip, float time,
                                   const std::vector<uint32>& usedJointIds)
{
    if (clip.GetFrames().empty())
        return;

    currentPose.frames = getPreviousAndNextFrames(clip, time);
    float progression  = calculateProgression(*currentPose.frames.first, *currentPose.frames.second, time);
    interpolatePoses(currentPose, *currentPose.frames.first, *currentPose.frames.second, progression, usedJointIds);
}

Animation::KeyFrame convert(const Pose& pose, float timestamp)
{
    Animation::KeyFrame result;
    result.timeStamp.value = timestamp;

    for (const auto& pair : pose.data)
    {
        result.transforms.insert({pair.first, pair.second.transform});
    }
    return result;
}
}  // namespace Components
}  // namespace GameEngine
