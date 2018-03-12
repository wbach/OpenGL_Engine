#include "AnimationUtils.h"
#include "GLM/GLMUtils.h"

namespace GameEngine
{
	namespace Animation
	{
		mat4 GetLocalTransform(const JointTransform& jt)
		{
			mat4 m(1.f);
			m *= glm::translate(jt.position);
			return m * glm::mat4_cast(jt.rotation);
		}
		const Joint* GetJoint(const Joint& from, uint32 fid)
		{
			if (from.id == fid)
				return &from;

			for (auto& j : from.children)
			{
				auto f = GetJoint(j, fid);
				if (f != nullptr)
					return f;
			}
			return nullptr;
		}
		void CalcInverseBindTransform(Joint& joint, const mat4& parentBindTransform)
		{
			mat4 bindTransform = parentBindTransform * joint.transform;
			joint.invtransform = glm::inverse(bindTransform);

			for (Joint& child : joint.children)
			{
				CalcInverseBindTransform(child, bindTransform);
			}
		}
		JointTransform Interpolate(const JointTransform & frameA, const JointTransform & frameB, float progress)
		{
			JointTransform out;
			out.position = glm::mix(frameA.position, frameB.position, progress);
			out.rotation = Utils::Interpolate(frameA.rotation, frameB.rotation, progress);
			return out;
		}
	} // Animation
} // GameEngine
