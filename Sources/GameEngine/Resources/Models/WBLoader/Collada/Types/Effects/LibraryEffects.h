#pragma once
#include "Effect.h"
#include <unordered_map>

namespace GameEngine
{
	namespace Collada
	{
		struct LibraryEffects
		{
			std::unordered_map<std::string, Effect> effects_;
		};
	}
}