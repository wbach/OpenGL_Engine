#pragma once
#include "Types.h"
#include <vector>

namespace GameEngine
{
	namespace Animation
	{
		struct Joint
		{
			uint32 id = 0;
			uint32 size = 0;
			std::string name = "";

			mat4 transform = glm::mat4(1.f);
			mat4 invtransform = glm::mat4(1.f);
			mat4 animatedTransform = glm::mat4(1.f);

			std::vector<Joint> children;
		};
	} // Animation
} // GameEngine
