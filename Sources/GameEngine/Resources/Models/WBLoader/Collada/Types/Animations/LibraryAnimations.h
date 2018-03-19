#pragma once
#include "Types.h"
#include "Animation.h"
#include <unordered_map>

namespace GameEngine
{
	namespace Collada
	{
		struct AnimationClip
		{
			std::unordered_map<std::string, Animation> animations_;
		};

		struct LibraryAnimations
		{
			std::unordered_map<std::string, AnimationClip> animationsClips_;
		};
	}
}
