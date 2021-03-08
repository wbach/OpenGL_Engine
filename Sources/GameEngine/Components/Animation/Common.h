#pragma once
#include <Types.h>

#include <unordered_map>

#include "AnimationClipInfo.h"
#include "GameEngine/Animations/AnimationClip.h"
#include "GameEngine/Animations/JointId.h"
#include "GameEngine/Animations/JointTransform.h"
#include "GameEngine/Animations/KeyFrame.h"

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
typedef std::unordered_map<std::string, std::vector<std::string>> JointGroups;
typedef std::unordered_map<std::string, std::vector<uint32>> JointGroupsIds;
typedef std::unordered_map<std::string, std::pair<AnimationClipInfo, float>> AnimationClipInfoPerGroup;

void interpolatePoses(Pose&, const Animation::KeyFrame&, const Animation::KeyFrame&, float);
void interpolatePoses(Pose&, const Animation::KeyFrame&, const Animation::KeyFrame&, float, const std::vector<uint32>&);
float calculateProgression(const Animation::KeyFrame&, const Animation::KeyFrame&, float);
std::pair<Animation::KeyFrame, Animation::KeyFrame> getPreviousAndNextFrames(const Animation::AnimationClip&, float);
void calculateCurrentAnimationPose(Pose&, const Animation::AnimationClip&, float);
void calculateCurrentAnimationPose(Pose&, const Animation::AnimationClip&, float, const std::vector<uint32>&);
std::pair<Animation::KeyFrame, Animation::KeyFrame> getPreviousAndNextFrames(const Animation::AnimationClip&, float);
Animation::KeyFrame convert(const Pose&, float = 0.f);
}  // namespace Components
}  // namespace GameEngine
