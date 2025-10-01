#pragma once
#include <optional>

#include "AnimationClip.h"
#include "JointTransform.h"

namespace GameEngine
{
class File;
namespace Animation
{
struct Joint;
mat4 GetLocalTransform(const JointTransform& jt);
JointTransform GetJointTransform(const glm::mat4& m);
const Joint* GetJoint(const Joint& from, uint32 fid);
void CalcInverseBindTransform(Joint& joint, const mat4& parentBindTransform = mat4(1.f));
JointTransform Interpolate(const JointTransform& frameA, const JointTransform& frameB, float progress);
std::optional<AnimationClip> ReadAnimationClip(const File&, Joint&);
void ExportAnimationClipToFile(const File&, const AnimationClip&, const Joint&, const std::string& name = "");
std::optional<std::string> IsAnimationClip(const File&);
// void ConvertAnimationClipToOtherSkeleton(const File&, const File&, )
}  // namespace Animation
}  // namespace GameEngine
