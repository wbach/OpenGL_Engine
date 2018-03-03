#pragma once
#include "Types.h"
#include "Image.h"
#include <unordered_map>

namespace GameEngine
{
	namespace Collada
	{
		struct LibraryImages
		{
			std::unordered_map<std::string, Image> images_;
		};
	}
}
