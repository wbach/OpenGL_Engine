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
typedef std::pair<Animation::KeyFrame const *, Animation::KeyFrame const *> CurrentFrames;

struct PoseData
{
    Animation::JointTransform transform;
    mat4 matrix;
};

struct Pose
{
    CurrentFrames frames;
    std::unordered_map<Animation::JointId, PoseData> data;
};


typedef std::unordered_map<std::string, std::vector<std::string>> JointGroups;
typedef std::unordered_map<std::string, std::vector<uint32>> JointGroupsIds;
typedef std::unordered_map<std::string, std::pair<const AnimationClipInfo&, float>> AnimationClipInfoPerGroup;

void interpolatePoses(Pose&, const Animation::KeyFrame&, const Animation::KeyFrame&, float);
void interpolatePoses(Pose&, const Animation::KeyFrame&, const Animation::KeyFrame&, float, const std::vector<uint32>&);
float calculateProgression(const Animation::KeyFrame&, const Animation::KeyFrame&, float);
CurrentFrames getPreviousAndNextFrames(const Animation::AnimationClip&, float);
void calculateCurrentAnimationPose(Pose&, const Animation::AnimationClip&, float);
void calculateCurrentAnimationPose(Pose&, const Animation::AnimationClip&, float, const std::vector<uint32>&);
Animation::KeyFrame convert(const Pose&, float = 0.f);
}  // namespace Components
}  // namespace GameEngine
