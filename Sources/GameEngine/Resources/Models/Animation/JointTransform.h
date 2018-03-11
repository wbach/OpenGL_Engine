#pragma once
#include "Types.h"
#include "GLM/GLMUtils.h"

namespace GameEngine
{
	namespace Animation
	{
		struct JointTransform
		{
			vec3 position;
			Quaternion rotation;
			mat4 readTransform;

			mat4 GetLocalTransform();
			static JointTransform Interpolate(const JointTransform& frameA, const JointTransform& frameB, float progress);
		};
	} // Animation
} // GameEngine