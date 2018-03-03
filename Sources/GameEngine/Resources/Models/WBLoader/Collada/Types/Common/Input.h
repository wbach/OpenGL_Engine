#pragma once
#include "Types.h"

namespace GameEngine
{
	namespace Collada
	{
		struct Input
		{
			std::string semantic;
			std::string sourceId;
			uint32 offset;
		};
	}
}