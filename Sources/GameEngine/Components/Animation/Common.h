#pragma once
#include <Types.h>
#include <unordered_map>
#include "GameEngine/Animations/KeyFrame.h"
#include "GameEngine/Animations/AnimationClip.h"

namespace GameEngine
{
namespace Components
{
typedef std::unordered_map<std::string, mat4> Pose;

Pose interpolatePoses(const Animation::KeyFrame&, const Animation::KeyFrame&, float);
float calculateProgression(const Animation::KeyFrame&, const Animation::KeyFrame&, float);
std::pair<Animation::KeyFrame, Animation::KeyFrame> getPreviousAndNextFrames(const Animation::AnimationClip&, float);
Pose calculateCurrentAnimationPose(const Animation::AnimationClip&, float);
Animation::KeyFrame convert(const Pose&, float = 0.f);
}  // namespace Components
}  // namespace GameEngine
