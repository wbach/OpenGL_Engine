#include "JointTransform.h"

Quaternion interpolate(Quaternion a, Quaternion b, float blend) {
	Quaternion result(0, 0, 0, 1);
	float dot = a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;
	float blendI = 1.f - blend;
	if (dot < 0) {
		result.w = blendI * a.w + blend * -b.w;
		result.x = blendI * a.x + blend * -b.x;
		result.y = blendI * a.y + blend * -b.y;
		result.z = blendI * a.z + blend * -b.z;
	}
	else {
		result.w = blendI * a.w + blend * b.w;
		result.x = blendI * a.x + blend * b.x;
		result.y = blendI * a.y + blend * b.y;
		result.z = blendI * a.z + blend * b.z;
	}
	result = glm::normalize(result);
	return result;
}

namespace GameEngine
{
	namespace Animation
	{
		mat4 JointTransform::GetLocalTransform()
		{
			mat4 m(1.f);
			m *= glm::translate(position);
			return m * glm::mat4_cast(rotation);
		}
		JointTransform JointTransform::Interpolate(const JointTransform & frameA, const JointTransform & frameB, float progress)
		{
			JointTransform out;
			out.position = glm::mix(frameA.position, frameB.position, progress);
			out.rotation = interpolate(frameA.rotation, frameB.rotation, progress);
			return out;
		}
	} // Animation
} // GameEngine
