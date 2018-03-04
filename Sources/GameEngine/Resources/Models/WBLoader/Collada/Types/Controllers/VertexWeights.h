#pragma once
#include "Types.h"
#include "GameEngine/Resources/Models/WBLoader/Collada/Types/Common/Input.h"
#include <vector>

namespace GameEngine
{
	namespace Collada
	{
		struct VertexWeights
		{
			uint32 count;
			std::vector<Input> inputs_;
			std::vector<uint32> vcount_;
			std::vector<uint32> v_;
		};
	}
}
