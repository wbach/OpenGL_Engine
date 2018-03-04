#pragma once
#include "Types.h"
#include "Node.h"
#include <unordered_map>

namespace GameEngine
{
	namespace Collada
	{
		struct VisualScene
		{
			std::string id_;
			std::string name_;
			std::unordered_map<std::string, Node> nodes_;
		};
	}
}
