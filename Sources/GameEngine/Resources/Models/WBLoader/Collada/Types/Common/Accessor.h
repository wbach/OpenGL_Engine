#pragma once
#include "Types.h"
#include "Param.h"
#include <list>

namespace GameEngine
{
	namespace Collada
	{
		struct Accessor
		{
			std::string source;
			uint32 stride;
			uint32 count;
			std::list<Param> params;
		};
	}
}