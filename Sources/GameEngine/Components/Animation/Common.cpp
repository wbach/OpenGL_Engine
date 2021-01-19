#include "Common.h"

#include "GameEngine/Animations/AnimationUtils.h"
#include "GameEngine/Animations/JointTransform.h"

namespace GameEngine
{
namespace Components
{
Pose interpolatePoses(const Animation::KeyFrame& previousFrame, const Animation::KeyFrame& nextFrame, float progression)
{
    Pose currentPose;
    for (const auto& pair : previousFrame.transforms)
    {
        const auto& jointName              = pair.first;
        const auto& nextFrameTransformIter = nextFrame.transforms.find(jointName);

        if (nextFrameTransformIter != nextFrame.transforms.cend())
        {
            const auto& previousTransform = pair.second;
            const auto& nextTransform     = nextFrameTransformIter->second;
            auto currentTransform         = Interpolate(previousTransform, nextTransform, progression);
            currentPose.insert({jointName, GetLocalTransform(currentTransform)});
        }
    }
    return currentPose;
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
Pose calculateCurrentAnimationPose(const Animation::AnimationClip& clip, float time)
{
    auto frames       = getPreviousAndNextFrames(clip, time);
    float progression = calculateProgression(frames.first, frames.second, time);
    return interpolatePoses(frames.first, frames.second, progression);
}
Animation::KeyFrame convert(const Pose& pose, float timestamp)
{
    Animation::KeyFrame result;
    result.timeStamp = timestamp;

    for (const auto& pair : pose)
    {
        result.transforms.insert({pair.first, Animation::GetJointTransform(pair.second)});
    }
    return result;
}
}  // namespace Components
}  // namespace GameEngine
