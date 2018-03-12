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
		};
	} // Animation
} // GameEngine