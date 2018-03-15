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
			uint32 size;
			std::string name;

			mat4 transform;
			mat4 invtransform;
			mat4 animatedTransform;

			std::vector<Joint> children;
		};
	} // Animation
} // GameEngine
