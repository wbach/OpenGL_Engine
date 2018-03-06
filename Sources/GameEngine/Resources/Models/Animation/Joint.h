#pragma once
#include "Types.h"
#include <vector>

namespace GameEngine
{
	namespace Animation
	{
		struct Joint
		{
			uint32 id;
			mat4 transform;
			mat4 invTransform;
			std::string name;
			std::vector<Joint> children;
		};
	} // Animation
} // GameEngine
