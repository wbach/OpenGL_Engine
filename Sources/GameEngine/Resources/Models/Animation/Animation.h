#pragma once
#include "KeyFrame.h"

namespace GameEngine
{
	namespace Animation
	{
		struct Animation
		{
			float length = 0.8333333f;
			std::vector<KeyFrame> frames;
		};
	} // Animation
} // GameEngine