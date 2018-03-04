#pragma once
#include "Types.h"
#include "VisualScene.h"
#include <unordered_map>

namespace GameEngine
{
	namespace Collada
	{
		struct LibraryVisualScenes
		{
			std::unordered_map<std::string, VisualScene> visualScenes_;
		};
	}
}
