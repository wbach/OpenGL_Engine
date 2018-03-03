#pragma once
#include "Types.h"
#include "GameEngine/Resources/Models/WBLoader/Collada/Types/Common/Source.h"
#include "Polylist.h"
#include "Vertices.h"
#include <unordered_map>

namespace GameEngine
{
	namespace Collada
	{
		struct Mesh
		{
			std::unordered_map<std::string, Source> sources_;
			std::unordered_map<std::string, Vertices> vertices_;
			std::unordered_map<std::string, Polylist> polylist_;
		};
	}
}