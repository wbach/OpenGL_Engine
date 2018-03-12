#pragma once
#include "Joint.h"
#include "JointTransform.h"

namespace GameEngine
{
	namespace Animation
	{
		mat4 GetLocalTransform(const JointTransform& jt);
		const Joint* GetJoint(const Joint& from, uint32 fid);
		void CalcInverseBindTransform(Joint& joint, const mat4& parentBindTransform);
		JointTransform Interpolate(const JointTransform& frameA, const JointTransform& frameB, float progress);
	} // Animation
} // GameEngine