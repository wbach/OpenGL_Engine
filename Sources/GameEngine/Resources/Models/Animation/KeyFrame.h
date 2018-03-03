#pragma once
#include "JointTransform.h"
#include <vector>

namespace GameEngine
{
	namespace Animation
	{
		struct KeyFrame
		{
			float timeStamp;
			std::vector<JointTransform> transforms;
		};
	} // Animation
} // GameEngine