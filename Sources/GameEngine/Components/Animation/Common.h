#pragma once
#include <Types.h>

#include <unordered_map>

#include "GameEngine/Animations/AnimationClip.h"
#include "GameEngine/Animations/KeyFrame.h"
#include "GameEngine/Animations/JointId.h"
#include "GameEngine/Animations/JointTransform.h"

namespace GameEngine
{
namespace Components
{
struct PoseData
{
    Animation::JointTransform transform;
    mat4 matrix;
};

typedef std::unordered_map<Animation::JointId, PoseData> Pose;

void interpolatePoses(Pose& currentPose, const Animation::KeyFrame&, const Animation::KeyFrame&, float);
float calculateProgression(const Animation::KeyFrame&, const Animation::KeyFrame&, float);
std::pair<Animation::KeyFrame, Animation::KeyFrame> getPreviousAndNextFrames(const Animation::AnimationClip&, float);
void calculateCurrentAnimationPose(Pose&, const Animation::AnimationClip&, float);
void calculateCurrentAnimationPose(Pose&, const Animation::AnimationClip&, float, const std::vector<uint32>&);
Animation::KeyFrame convert(const Pose&, float = 0.f);
}  // namespace Components
}  // namespace GameEngine
