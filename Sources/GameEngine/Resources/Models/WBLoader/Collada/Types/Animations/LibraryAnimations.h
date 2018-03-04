#pragma once
#include "Types.h"
#include "Animation.h"
#include <unordered_map>

namespace GameEngine
{
	namespace Collada
	{
		struct LibraryAnimations
		{
			std::unordered_map<std::string, Animation> animations_;
		};
	}
}
