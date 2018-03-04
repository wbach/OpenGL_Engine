#pragma once
#include "Types.h"
#include "Controller.h"
#include <unordered_map>

namespace GameEngine
{
	namespace Collada
	{
		struct LibraryControllers
		{
			std::unordered_map<std::string, Controller> controllers_;
		};
	}
}
