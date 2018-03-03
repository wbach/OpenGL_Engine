#pragma once
#include "Types.h"
#include "GameEngine/Resources/Models/WBLoader/Collada/Types/Common/Input.h"
#include <vector>

namespace GameEngine
{
	namespace Collada
	{
		struct Vertices
		{
			std::vector<Input> inputs_;
		};
	}
}