#pragma once
#include "Joint.h"
#include "JointTransform.h"
#include "AnimationClip.h"
#include "GameEngine/Resources/File.h"

namespace GameEngine
{
namespace Animation
{
mat4 GetLocalTransform(const JointTransform& jt);
JointTransform GetJointTransform(const glm::mat4& m);
const Joint* GetJoint(const Joint& from, uint32 fid);
void CalcInverseBindTransform(Joint& joint, const mat4& parentBindTransform = mat4(1.f));
JointTransform Interpolate(const JointTransform& frameA, const JointTransform& frameB, float progress);
AnimationClip ReadAnimationClip(const File&);
void ExportAnimationClipToFile(const File&, const AnimationClip&);
//void ConvertAnimationClipToOtherSkeleton(const File&, const File&, )
}  // namespace Animation
}  // namespace GameEngine
