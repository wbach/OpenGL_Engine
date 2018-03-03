#pragma once
#include "Types.h"
#include "GameEngine/Resources/Models/WBLoader/Collada/Types/Common/Input.h"
#include <vector>

namespace GameEngine
{
	namespace Collada
	{
		struct Polylist
		{
			uint32 count_;
			std::string materialId_;
			std::vector<Input> inputs_;
			std::vector<uint32> vcount_;
			std::vector<uint32> p_;
		};
	}
}
