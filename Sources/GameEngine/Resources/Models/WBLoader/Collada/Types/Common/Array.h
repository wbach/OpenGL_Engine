#pragma once
#include "Types.h"

namespace GameEngine
{
	namespace Collada
	{
		struct Array
		{
			std::string data;
			std::string id;
			std::string type;
			uint32 count;
		};
	}
}