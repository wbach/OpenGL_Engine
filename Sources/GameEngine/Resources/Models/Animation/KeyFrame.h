#pragma once
#include "JointTransform.h"
#include <unordered_map>

namespace GameEngine
{
	namespace Animation
	{
		struct KeyFrame
		{
			float timeStamp;
			std::unordered_map<std::string, JointTransform> transforms;
			std::unordered_map<uint32, JointTransform> idTransforms_;
		};
	} // Animation
} // GameEngine