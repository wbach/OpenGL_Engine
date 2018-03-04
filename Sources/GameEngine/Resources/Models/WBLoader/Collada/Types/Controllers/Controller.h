#pragma once
#include "Types.h"
#include "Skin.h"

namespace GameEngine
{
	namespace Collada
	{
		struct Controller
		{
			std::string id_;
			std::string name_;
			std::unordered_map<std::string, Skin> skins_;
		};
	}
}
