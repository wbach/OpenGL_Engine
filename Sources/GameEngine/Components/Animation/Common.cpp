#include "Common.h"

#include "GameEngine/Animations/AnimationUtils.h"
#include "GameEngine/Animations/JointTransform.h"

namespace GameEngine
{
namespace Components
{
void interpolatePoses(Pose& currentPose, const Animation::KeyFrame& previousFrame, const Animation::KeyFrame& nextFrame, float progression)
{
    for (const auto& pair : previousFrame.transforms)
    {
        const auto& jointId                = pair.first;
        const auto& nextFrameTransformIter = nextFrame.transforms.find(jointId);

        if (nextFrameTransformIter != nextFrame.transforms.cend())
        {
            auto& poseData = currentPose[jointId];
            const auto& previousTransform = pair.second;
            const auto& nextTransform     = nextFrameTransformIter->second;
            poseData.transform = Interpolate(previousTransform, nextTransform, progression);
            poseData.matrix = GetLocalTransform(poseData.transform);
        }
    }
}
float calculateProgression(const Animation::KeyFrame& previousFrame, const Animation::KeyFrame& nextFrame, float time)
{
    float totalTime   = nextFrame.timeStamp - previousFrame.timeStamp;
    float currentTime = time - previousFrame.timeStamp;
    return glm::clamp(currentTime / totalTime, 0.f, 1.f);
}
std::pair<Animation::KeyFrame, Animation::KeyFrame> getPreviousAndNextFrames(const Animation::AnimationClip& clip,
                                                                             float time)
{
    const auto& allFrames = clip.GetFrames();

    auto previousFrame = allFrames[0];
    auto nextFrame     = allFrames[0];

    for (uint32 i = 1; i < allFrames.size(); i++)
    {
        nextFrame = allFrames[i];
        if (nextFrame.timeStamp > time)
            break;

        previousFrame = allFrames[i];
    }
    return {previousFrame, nextFrame};
}
void calculateCurrentAnimationPose(Pose& currentPose, const Animation::AnimationClip& clip, float time)
{
    auto frames       = getPreviousAndNextFrames(clip, time);
    float progression = calculateProgression(frames.first, frames.second, time);
    interpolatePoses(currentPose, frames.first, frames.second, progression);
}
Animation::KeyFrame convert(const Pose& pose, float timestamp)
{
    Animation::KeyFrame result;
    result.timeStamp = timestamp;

    for (const auto& pair : pose)
    {
        result.transforms.insert({pair.first, pair.second.transform});
    }
    return result;
}
}  // namespace Components
}  // namespace GameEngine
