#pragma once
#include "Types.h"
#include "Array.h"
#include "TechniqueCommon.h"

namespace GameEngine
{
	namespace Collada
	{
		struct Source
		{
			std::string id;
			TechniqueCommon technique;
			Array dataArray;
		};
	}
}