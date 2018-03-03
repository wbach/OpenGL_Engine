#pragma once
#include "Types.h"
#include "Material.h"
#include <unordered_map>

namespace GameEngine
{
	namespace Collada
	{
		struct LibraryMaterials
		{
			std::unordered_map<std::string, Material> materials_;
		};
	}
}
