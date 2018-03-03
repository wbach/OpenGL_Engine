#pragma once
#include "Types.h"
#include "Geometry.h"
#include <unordered_map>

namespace GameEngine
{
	namespace Collada
	{
		struct LibraryGeometries
		{
			std::unordered_map<std::string, Geometry> geometries_;
		};
	}
}